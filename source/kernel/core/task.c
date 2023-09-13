/**
 * @file task.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  用TSS来保存任务状态从而实现线程切换
 * @version 0.1
 * @date 2023-01-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "core/task.h"

#include "common/cpu_instr.h"
#include "common/elf.h"
#include "core/memory.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/mmu.h"
#include "cpu/syscall.h"
#include "fs/fs.h"
#include "os_cfg.h"
#include "tools/assert.h"
#include "tools/klib.h"
#include "tools/log.h"

// 定义全局唯一的任务管理器对象
static task_manager_t task_manager;
// 定义静态的任务表，用于任务的静态分配
static task_t task_table[TASK_COUNT];
// 定义用于维护task_table的互斥锁
static mutex_t task_table_lock;

/**
 * @brief 根据文件描述符从当前任务进程的打开文件表中返回对应的文件结构指针
 *
 * @param fd 文件描述符
 * @return file_t*
 */
file_t *task_file(int fd) {
  file_t *file = (file_t *)0;

  if (fd >= 0 && fd < TASK_OFILE_SIZE) {
    file = task_current()->file_table[fd];
  }

  return file;
}

/**
 * @brief 将已分配的文件结构指针放入当前进程的打开文件表中，并返回文件描述符
 *
 * @param file 已从系统file_table中分配的文件结构指针
 * @return int 文件描述符
 */
int task_alloc_fd(file_t *file) {
  task_t *task = task_current();
  for (int i = 0; i < TASK_OFILE_SIZE; ++i) {
    file_t *p = task->file_table[i];
    if (p == (file_t *)0) {  // 打开文件表中的第i项未分配，对其进行分配操作
      task->file_table[i] = file;
      return i;
    }
  }

  return -1;
}

/**
 * @brief 从当前进程的打开文件表中移除文件描述符对应的文件结构指针
 *
 * @param fd
 */
void task_remove_fd(int fd) {
  // 清空文件描述符对应的内存资源即可
  if (fd >= 0 && fd < TASK_OFILE_SIZE) {
    task_current()->file_table[fd] = (file_t *)0;
  }
}

static void switch_to_tss(uint16_t tss_selector) {
  // 进行远跳转，让cpu访问该tss段的描述符，cpu将重启到之前运行该tss对应的任务的状态继续运行该任务
  far_jump(tss_selector, 0);
}

/**
 * @brief  将任务从from切换到to
 *
 * @param from 切换前的任务
 * @param to 切换后的任务
 */
void task_switch_from_to(task_t *from, task_t *to) {
  // 跳转到对应的tss段读取并恢复cpu任务状态
  switch_to_tss(to->tss_selector);
}

/**
 * @brief  初始化任务的tss段
 *
 * @param task 需要初始化的任务对象
 * @param entry 任务入口地址
 * @param esp 任务所用的栈顶指针
 * @param flag 任务属性标志位，如特权级
 */
static int tss_init(task_t *task, uint32_t entry, uint32_t esp, uint32_t flag) {
  // 1.将该TSS段绑定到GDT中的某个段描述符
  uint32_t tss_selector = gdt_alloc_desc();
  if (tss_selector < 0) {
    log_printf("alloc tss failed!\n");
    return -1;
  }
  segment_desc_set(tss_selector, (uint32_t)&task->tss, sizeof(task->tss),
                   SEG_ATTR_P | SEG_ATTR_DPL_0 | SEG_ATTR_TYPE_TSS);

  // 2.将tss段的值置空
  kernel_memset(&task->tss, 0, sizeof(task->tss));

  // 3.记录tss绑定到的描述符的选择子
  task->tss_selector = tss_selector;

  // 4.根据任务的特权级来设置对应选择子的cpl
  uint32_t code_selector, data_selector;
  if (flag & TASK_FLAGS_SYSTEM) {  // 内核特权级
    code_selector = KERNEL_SELECTOR_CS;
    data_selector = KERNEL_SELECTOR_DS;
  } else {  // 用户特权级
    // 获取任务的代码段选择子和数据段选择子，并初始化其CPL,因为是平台模型，所以基本不涉及到跨段访问，所以不需要RPL
    code_selector = task_manager.app_code_selector | SEG_CPL3;
    data_selector = task_manager.app_data_selector | SEG_CPL3;
  }

  // 5.设置eip，即任务的起始地址
  task->tss.eip = entry;

  // 6.根据任务的特权级设置任务所使用的栈空间
  task->tss.esp = esp;  // 特权级为3的栈空间
  uint32_t kernel_stack =
      memory_alloc_page();  // 分配一页作为内核特权级0的栈空间
  if (kernel_stack == 0)    // 内核栈空间初始化失败
    goto tss_init_failed;
  else
    task->tss.esp0 = kernel_stack + MEM_PAGE_SIZE;  // 特权级为0的栈空间

  // 7.平坦模型，初始化栈空间段寄存器
  task->tss.ss = data_selector;  // 特权级为3时使用的栈段
  task->tss.ss0 =
      KERNEL_SELECTOR_DS;  // 特权级为0时使用的栈段，
                           // 由于平坦模型，其实使用的是同一片空间，只是特权级发生了变化

  // 8. 平坦模型，初始其余化段寄存器
  task->tss.es = task->tss.fs = task->tss.gs = task->tss.ds = data_selector;

  // 9.平坦模型，初始化代码段寄存器
  task->tss.cs = code_selector;

  // 10.初始化eflags寄存器，使cpu中断保持开启
  task->tss.eflags = EFLAGS_DEFAULT_1 | EFLAGS_IF;

  // 11.创建当前进程的虚拟页目录表，并设置cr3寄存器
  uint32_t page_dir = memory_creat_uvm();
  if (page_dir == 0) goto tss_init_failed;
  task->tss.cr3 = page_dir;

  return 0;

// tss初始化失败
tss_init_failed:
  gdt_free(tss_selector);  // 释放选择子

  if (kernel_stack) {  // 内核栈空间分配有效，需要释放
    memory_free_page(kernel_stack);
  }
  return -1;
}

/**
 * @brief 将任务插入任务链表中并设为就绪态，标志该任务可被调度
 *
 * @param task
 */
void task_start(task_t *task) {
  idt_state_t state = idt_enter_protection();  // TODO:加锁

  //将任务设置为就绪态
  task_set_ready(task);
  task->state = TASK_READY;

  idt_leave_protection(state);  // TODO:解锁
}

/**
 * @brief  初始化任务
 *
 * @param task 任务对象
 * @param entry 任务的入口地址
 * @param esp 任务指行时所用的栈顶指针
 * @param flag 任务属性标志位，如特权级
 * @return int
 */
int task_init(task_t *task, const char *name, uint32_t entry, uint32_t esp,
              uint32_t flag) {
  ASSERT(task != (task_t *)0);
  // 1.初始化任务TSS段
  int err = tss_init(task, entry, esp, flag);
  if (err == -1) return err;

  // 2.初始化任务名称
  kernel_strncpy(task->name, name, TASK_NAME_SIZE);

  // 3.初始化任务队列节点及就绪队列节点
  list_node_init(&task->ready_node);
  list_node_init(&task->task_node);
  list_node_init(&task->wait_node);

  // 4.初始化最大时间片数与当前拥有时间片数,以及延时时间片数
  task->state = TASK_CREATED;
  task->slice_max = task->slice_curr = TASK_TIME_SLICE_DEFAULT;
  task->sleep = 0;
  task->pid = (uint32_t)task;
  task->parent = (task_t *)0;
  task->heap_start = task->heap_end = 0;
  task->status = 0;

  // 5.初始化文件表
  kernel_memset(&task->file_table, 0, sizeof(task->file_table));

  // 6.将任务加入任务队列
  list_insert_last(&task_manager.task_list, &task->task_node);

  return 1;
}


static void free_task(task_t* task);
/**
 * @brief 反初始化任务对象，释放对应的资源
 *
 * @param task
 */
void task_uninit(task_t *task) {
  //释放已分配的选择子
  if (task->tss_selector) {  
    gdt_free(task->tss_selector);
  }

  //释放已分配的内核栈空间
  if (task->tss.esp0) {  
    memory_free_page((uint32_t)(task->tss.esp0 - MEM_PAGE_SIZE));
  }
  
  //释放为页目录分配的页空间及其映射关系
  if (task->tss.cr3) {  
    memory_destroy_uvm(task->tss.cr3);
  }


  //将任务结构从任务管理器的任务队列中取下
  list_remove(&task_manager.task_list, &task->task_node);
  
  //释放全局任务表中的task结构资源
  free_task(task);
}

// 空闲进程的栈空间
static uint32_t empty_task_stack[EMPTY_TASK_STACK_SIZE];
/**
 * @brief  空闲进程，当所有进程都延时运行时，让cpu运行空闲进程
 *
 */
static void empty_task(void) {
  while (1) {
    // 停止cpu运行，让cpu等待时间中断
    hlt();
  };
}

/**
 * @brief  初始化任务管理器
 *
 */
void task_manager_init(void) {
  // 1.初始化应用程序及所有任务的代码段选择子和数据段选择子，以区分和内核选择子的特权级
  // 应用程序运行在 DPL_3 下，
  // 内核运行在DPL_0下，配合分页机制中的us位，0为内核特权级，1为用户特权级
  // 就可做到特权级保护
  uint32_t data_selector = gdt_alloc_desc();
  uint32_t code_selector = gdt_alloc_desc();
  segment_desc_set(data_selector, 0, 0xffffffff,
                   SEG_ATTR_P | SEG_ATTR_DPL_3 | SEG_ATTR_S_NORMAL |
                       SEG_ATTR_TYPE_DATA | SEG_ATTR_TYPE_RW | SEG_ATTR_D_OR_B);
  segment_desc_set(code_selector, 0, 0xffffffff,
                   SEG_ATTR_P | SEG_ATTR_DPL_3 | SEG_ATTR_S_NORMAL |
                       SEG_ATTR_TYPE_CODE | SEG_ATTR_TYPE_RW | SEG_ATTR_D_OR_B);
  task_manager.app_code_selector = code_selector;
  task_manager.app_data_selector = data_selector;

  // 2.初始化所有任务队列
  list_init(&task_manager.ready_list);
  list_init(&task_manager.task_list);
  list_init(&task_manager.sleep_list);

  // 3.将当前任务置零
  task_manager.curr_task = (task_t *)0;

  // 4.初始化空闲进程
  task_init(&task_manager.empty_task, "empty_task", (uint32_t)empty_task,
            (uint32_t)&empty_task_stack[EMPTY_TASK_STACK_SIZE],
            TASK_FLAGS_SYSTEM);

  // 5.初始化静态任务表,及其互斥锁
  kernel_memset(task_table, 0, sizeof(task_table));
  mutex_init(&task_table_lock);
}

/**
 * @brief  初始化第一个任务
 *
 */
// void task_first_init(void) {
//       //1.初始化任务，当前任务是在任务管理器启用前就执行的，
//       //拥有自己的栈空间，所以入口地址直接和栈空间都置0即可
//       //这一步只是为当前任务绑定一个TSS段并将其绑定到一个task对象
//       task_init(&task_manager.first_task, "first task", 0, 0);

//       //2.将当前任务的TSS选择子告诉cpu，用来切换任务时保存状态
//       write_tr(task_manager.first_task.tss_selector);

//       //3.将当前任务执行第一个任务
//       task_manager.curr_task = &task_manager.first_task;

//       //4.将当前任务状态设置为运行态
//       task_manager.curr_task->state = TASK_RUNNING;
// }
/**
 * @brief 初始化第一个任务
 * 内存分部：
 *      用户区
 * -------------------
 *      stack段 = 5 页
 * -------------------
 *      code段
 *      rodata段
 * -------------------
 *      data段
 *      bss段
 * -------------------
 *      内核区
 * ------------------
 *     页目录表 = 1页
 * ------------------
 * 内核0特权级栈 = 1页
 * ------------------
 */
void task_first_init(void) {
  // 1.声明第一个任务的符号
  void first_task_entry(void);

  // 2.确定第一个任务进程需要的空间大小
  extern char s_first_task[], e_first_task[];
  uint32_t copy_size =
      (uint32_t)(e_first_task - s_first_task);  // 进程所需空间大小
  uint32_t alloc_size =
      up2(copy_size, MEM_PAGE_SIZE) +
      10 *
          MEM_PAGE_SIZE;  // 需要为进程分配的内存大小，按4kb对齐,并多拿五页当作栈空间
  ASSERT(copy_size < alloc_size);

  uint32_t task_start_addr =
      (uint32_t)first_task_entry;  // 获取第一个任务的入口地址

  // 3.初始化第一个任务,因为当前为操作系统进程，esp初始值随意赋值都可，
  //  因为当前进程已开启，cpu会在切换的时候保留真实的状态，即真实的esp值
  task_init(&task_manager.first_task, "first task", task_start_addr,
            task_start_addr + alloc_size, TASK_FLAGS_USER);

  // 4.初始化进程的起始堆空间
  task_manager.first_task.heap_start =
      (uint32_t)e_first_task;  // 堆起始地址紧靠程序bss段之后
  task_manager.first_task.heap_end = (uint32_t)e_first_task;  // 堆大小初始为0

  // 5.将当前任务的TSS选择子告诉cpu，用来切换任务时保存状态
  write_tr(task_manager.first_task.tss_selector);

  // 6.将当前任务执行第一个任务
  task_manager.curr_task = &task_manager.first_task;

  // 7.将当前页表设置为第一个任务的页表
  mmu_set_page_dir(task_manager.first_task.tss.cr3);

  // 8.将当前任务状态设置为运行态
  task_manager.curr_task->state = TASK_RUNNING;

  // 9.进程的各个段还只是在虚拟地址中，所以要为各个段分配物理地址页空间，并进行映射
  memory_alloc_page_for(task_start_addr, alloc_size, PTE_P | PTE_W | PTE_U);

  // 10.将任务进程各个段从内核四个段之后的紧邻位置，拷贝到已分配好的且与虚拟地址对应的物理地址空间，实现代码隔离
  kernel_memcpy(first_task_entry, s_first_task, alloc_size);

  // 11.将任务设为可被调度
  task_start(&task_manager.first_task);
}

/**
 * @brief  获取当前任务管理器的第一个任务
 *
 * @return task_t*
 */
task_t *task_first_task(void) { return &task_manager.first_task; }

/**
 * @brief  将任务task加入就绪队列
 *
 * @param task 需要加入就绪队列的任务
 */
void task_set_ready(task_t *task) {
  ASSERT(task != (task_t *)0);
  // if (task == (task_t*)0) return;
  // 1.将任务插入到就绪队列的尾部
  list_insert_last(&task_manager.ready_list, &task->ready_node);

  // 2.将任务状态设置为就绪态
  // task->state = TASK_READY;
}

/**
 * @brief  将任务task从就绪队列中取下
 *
 * @param task
 */
void task_set_unready(task_t *task) {
  ASSERT(task != (task_t *)0);
  // if (task == (task_t*)0) return;
  list_remove(&task_manager.ready_list, &task->ready_node);
}

/**
 * @brief  获取就绪队列中的第一个任务
 *
 */
task_t *task_ready_first(void) {
  list_node_t *ready_node = list_get_first(&task_manager.ready_list);

  return list_node_parent(ready_node, task_t, ready_node);
}

/**
 * @brief  获取当前正在运行的任务
 *
 * @return task_t*
 */
task_t *task_current(void) { return task_manager.curr_task; }

/**
 * @brief  任务管理器进行任务切换
 *
 */
void task_switch(void) {
  idt_state_t state = idt_enter_protection();  // TODO:加锁

  // 1.获取就绪队列中的第一个任务
  task_t *to = task_ready_first();

  // 2.若获取到的任务不是当前任务就进行切换
  if (to != task_manager.curr_task) {
    // 3.获取当前任务
    task_t *from = task_manager.curr_task;

    // 4.目标任务若为空，则所有任务都在延时，让cpu运行空闲任务
    if (to == (task_t *)0) {
      to = &task_manager.empty_task;
    }
    // 5.切换当前任务, 并将当前任务置为运行态
    to->state = TASK_RUNNING;
    if (from->state == TASK_RUNNING) {
      from->state = TASK_READY;
    } 
    task_manager.curr_task = to;

    // 6.进行任务切换
    task_switch_from_to(from, to);
  }

  idt_leave_protection(state);  // TODO:解锁
}

/**
 * @brief  提供给时钟中断使用，每中断一次，当前任务的时间片使用完一次
 *         减少当前任务的时间片数，并判断是否还有剩余时间片，若没有就进行任务切换
 *
 */
void task_slice_end(void) {
  // 1.遍历当前延时队列，判断是否有可唤醒的任务
  list_node_t *curr_sleep_node = list_get_first(&task_manager.sleep_list);

  // 2.遍历并判断每一个任务执行完当前时间片是否可被唤醒，若可以则唤醒
  while (curr_sleep_node) {
    list_node_t *next_sleep_node = list_node_next(curr_sleep_node);

    task_t *curr_sleep_task =
        list_node_parent(curr_sleep_node, task_t, ready_node);
    if (--curr_sleep_task->sleep == 0) {
      task_set_wakeup(curr_sleep_task);  // 从延时队列中取下
      task_set_ready(curr_sleep_task);   // 加入就绪队列
    }

    curr_sleep_node = next_sleep_node;
  }

  // task_switch(); 没有必要立马进行任务切换，当前任务时间片用完后会自动切换
  // 3.获取当前任务
  task_t *curr_task = task_current();

  // 4.若当前任务为空闲任务，则判断就绪队列是否为空
  if (curr_task == &task_manager.empty_task) {
    if (list_is_empty(&task_manager.ready_list)) return;

    task_manager.empty_task.state = TASK_CREATED;

    task_switch();  // 就绪队列有任务，则直接切换任务
  }

  // 5.若当前任务为普通任务则，减小当前时间片数
  if (curr_task != &task_manager.empty_task && --curr_task->slice_curr == 0) {
    // 6.时间片数用完了，重置时间片并进行任务切换
    curr_task->slice_curr = curr_task->slice_max;
    task_set_unready(curr_task);
    task_set_ready(curr_task);
    task_switch();
  }
}

/**
 * @brief  设置进程延时的时间片数
 *
 * @param task 需要延时的进程
 * @param slice 延时的时间片数
 */
void task_set_sleep(task_t *task, uint32_t slice) {
  ASSERT(task != (task_t *)0);
  if (slice == 0) return;

  task->sleep = slice;
  task->state = TASK_SLEEP;
  list_insert_last(&task_manager.sleep_list, &task->ready_node);
}

/**
 * @brief  唤醒正在延时的进程
 *
 * @param task
 */
void task_set_wakeup(task_t *task) {
  ASSERT(task != (task_t *)0);
  list_remove(&task_manager.sleep_list, &task->ready_node);
  task->state = TASK_CREATED;
}

/**
 * @brief 从静态任务表中分配一个任务对象
 *
 * @return task_t*
 */
static task_t *alloc_task(void) {
  task_t *task = 0;

  //TODO:加锁
  mutex_lock(&task_table_lock);

  // 遍历静态任务表，取出未被分配的任务对象空间
  for (int i = 0; i < TASK_COUNT; ++i) {
    task_t *curr = task_table + i;
    if (curr->pid == 0) {
      task = curr;
      break;
    }
  }

  //TODO:解锁
  mutex_unlock(&task_table_lock);

  return task;
}

/**
 * @brief 释放静态任务表的任务对象
 *
 * @param task
 */
static void free_task(task_t *task) {
  //TODO:加锁
  mutex_lock(&task_table_lock);

  task->pid = 0;
  task->parent = (task_t*)0;

  //TODO:解锁
  mutex_unlock(&task_table_lock);
}

/**
 * @brief  使进程进入延时状态
 *
 * @param ms 延时的时间，以ms为单位
 */
void sys_sleep(uint32_t ms) {
  idt_state_t state = idt_enter_protection();  // TODO:加锁

  // 1.获取当前任务
  task_t *curr_task = task_current();

  // 2.将当前任务离开就绪队列
  task_set_unready(curr_task);

  // 3.计算出需要延时的时间片数，对时间片数向上取整，保证进程至少能延时指定时间
  uint32_t slice = (ms + (OS_TICKS_MS - 1)) / OS_TICKS_MS;

  // 4.将当前任务放入延时队列，并设置延时时间片数
  task_set_sleep(curr_task, slice);

  // 5.切换任务
  task_switch();

  idt_leave_protection(state);  // TODO:解锁
}

/**
 * @brief 获取任务pid
 *
 * @return int pid
 */
int sys_getpid(void) { return task_current()->pid; }


/**
 * @brief 将当前进程的打开文件表复制给传入进程
 * 
 * @param child_task 
 */
static void copy_opened_files(task_t *child_task) {
  task_t *parent = task_current();
  for (int i = 0; i < TASK_OFILE_SIZE; ++i) {
    file_t *file = parent->file_table[i];
    if (file) {
      file_inc_ref(file);
      child_task->file_table[i] = file;
    }
  }
}

/**
 * @brief 创建子进程
 *
 * @return int 子进程的pid
 */
int sys_fork(void) {
  // 1.获取当前进程为fork进程的父进程
  task_t *parent_task = task_current();

  // 2.分配子进程控制块
  task_t *child_task = alloc_task();
  if (child_task == (task_t *)0) goto fork_failed;

  // 3.获取系统调用的栈帧,因为每次通过调用门进入内核栈中都只会一帧该结构体的数据，
  // 所以用最高地址减去大小即可获得该帧的起始地址
  syscall_frame_t *frame =
      (syscall_frame_t *)(parent_task->tss.esp0 - sizeof(syscall_frame_t));

  // 4.初始子进程控制块，直接用父进程进入调用门的下一条指令地址作为子进程的入口地址
  int err = task_init(child_task, parent_task->name, frame->eip,
                      frame->esp + sizeof(uint32_t) * SYSCALL_PARAM_COUNT,
                      TASK_FLAGS_USER);
  if (err < 0) goto fork_failed;

  //让子进程继承父进程的打开文件表
  copy_opened_files(child_task);


  // 5.恢复到父进程的上下文环境
  tss_t *tss = &(child_task->tss);
  // 子进程执行的第一条指令就是从eax中取出系统用的返回值，即进程id，子进程固定获取0
  tss->eax = 0;
  tss->ebx = frame->ebx;
  tss->ecx = frame->ecx;
  tss->edx = frame->edx;
  tss->edi = frame->edi;
  tss->esi = frame->esi;
  tss->eflags = frame->eflags;
  tss->ebp = frame->ebp;

  // 复用父进程的段寄存器
  tss->cs = frame->cs;
  tss->ds = frame->ds;
  tss->es = frame->es;
  tss->fs = frame->fs;
  tss->gs = frame->gs;
  tss->ss = frame->ss;

  // 记录父进程地址
  child_task->parent = parent_task;

  //记录父进程堆空间
  child_task->heap_start = parent_task->heap_start;
  child_task->heap_end = parent_task->heap_end;

  // 7.拷贝进程虚拟页目录表和页表，即拷贝其映射关系
  if (memory_copy_uvm(tss->cr3, parent_task->tss.cr3) < 0) goto fork_failed;

  // 8.子进程控制块初始化完毕，设为可被调度态
  task_start(child_task);
  // 反回子进程id
  return child_task->pid;

// fork失败，清理资源
fork_failed:
  if (child_task) {  // 初始化失败，释放对应资源
    task_uninit(child_task);
    free_task(child_task);
  }

  return -1;
}

/**
 * @brief  使当前正在运行的任务主动让出cpu
 *
 * @return int
 */
int sys_yield(void) {
  idt_state_t state = idt_enter_protection();  // TODO:加锁

  // 1.判断当前就绪队列中是否有多个任务
  if (list_get_size(&task_manager.ready_list) > 1) {
    // 2.获取当前任务
    task_t *curr_task = task_current();

    // 3.将当前任务从就绪队列中取下
    task_set_unready(curr_task);

    // 4.将当前任务重新加入到就绪队列的队尾
    task_set_ready(curr_task);

    // 5.任务管理器运行下一个任务，从而释放cpu使用权
    task_switch();
  }

  idt_leave_protection(state);  // TODO:解锁
  return 0;
}

/**
 * @brief 将elf文件的程序段表项对应的程序段加载到page_dir对应的地址空间中
 *
 * @param file elf文件描述符
 * @param elf_phdr  程序段表项
 * @param page_dir 需要加载到的目标空间的页目录表地址
 * @return int
 */
static int load_phdr(int file, Elf32_Phdr *elf_phdr, uint32_t page_dir) {
  // 获取该段的权限
  uint32_t privilege = PTE_P | PTE_U;
  if (elf_phdr->p_flags & PT_W) {  // 该段具有写权限
    privilege |= PTE_W;
  }

  // 为该段分配页空间并创建映射关系
  int err = memory_alloc_for_page_dir(page_dir, elf_phdr->p_vaddr,
                                      elf_phdr->p_memsz, privilege);
  if (err < 0) {
    log_printf("no memory\n");
    return -1;
  }

  // 使文件的读取位置偏移到该程序段的起始位置
  if (sys_lseek(file, elf_phdr->p_offset, 0) < 0) {
    log_printf("lseek file failed\n");
    return -1;
  }

  // 获取该程序段的起始虚拟地址和段在文件中的大小
  uint32_t vaddr = elf_phdr->p_vaddr;
  uint32_t size = elf_phdr->p_filesz;

  while (size > 0) {  // 按页读取并拷贝
    // 获取需要拷贝的空间大小
    int curr_size = (size > MEM_PAGE_SIZE) ? MEM_PAGE_SIZE : size;
    // 获取vaddr在page_dir中关联的物理页的物理地址
    uint32_t paddr = memory_get_paddr(page_dir, vaddr);

    // 拷贝curr_size大小的内容到paddr对应的页中
    if (sys_read(file, (char *)paddr, curr_size) < curr_size) {
      log_printf("read file failed\n");
      return -1;
    }

    size -= curr_size;
    vaddr += curr_size;
  }

  return 0;
}

/**
 * @brief 加载解析elf文件，并返回程序入口地址
 *
 * @param task
 * @param name
 * @param page_dir
 * @return uint32_t
 */
static uint32_t load_elf_file(task_t *task, const char *name,
                              uint32_t page_dir) {
  // 1.定义elf文件头对象,和程序段表项对象
  Elf32_Ehdr elf_hdr;
  Elf32_Phdr elf_phdr;

  // 2.打开文件
  int file = sys_open(name, 0);
  if (file < 0) {
    log_printf("open failed %s\n", name);
    goto load_failed;
  }

  // 3.读取elf文件的elf头部分
  int cnt = sys_read(file, (char *)&elf_hdr, sizeof(Elf32_Ehdr));
  if (cnt < sizeof(Elf32_Ehdr)) {
    log_printf("elf hdr too small. size=%d\n", cnt);
    goto load_failed;
  }

  // 4.判断是否为ELF文件
  if (elf_hdr.e_ident[0] != 0x7F || elf_hdr.e_ident[1] != 'E' ||
      elf_hdr.e_ident[2] != 'L' || elf_hdr.e_ident[3] != 'F') {
    log_printf("check elf ident failed.\n");
    goto load_failed;
  }

  // 5.必须是可执行文件和针对386处理器的类型，且有入口
  if ((elf_hdr.e_type != ET_EXEC) || (elf_hdr.e_machine != EM_386) ||
      (elf_hdr.e_entry == 0)) {
    log_printf("check elf type or entry failed.\n");
    goto load_failed;
  }

  // 6.必须有程序头部
  if ((elf_hdr.e_phentsize == 0) || (elf_hdr.e_phoff == 0)) {
    log_printf("none programe header\n");
    goto load_failed;
  }

  // 7.遍历elf文件的程序段，加载可加载段到内存中对应位置
  uint32_t e_phoff = elf_hdr.e_phoff;  // 获取程序段表的偏移地址
  for (int i = 0; i < elf_hdr.e_phnum; ++i, e_phoff += elf_hdr.e_phentsize) {
    if (sys_lseek(file, e_phoff, 0) < 0) {
      log_printf("read file failed\n");
      goto load_failed;
    }

    cnt = sys_read(file, (char *)&elf_phdr, sizeof(Elf32_Phdr));
    if (cnt < sizeof(Elf32_Phdr)) {
      log_printf("read file failed\n");
      goto load_failed;
    }

    // 若程序段不是可加载的或虚拟地址 < 用户程序的起始地址，则不可用
    if (elf_phdr.p_type != 1 || elf_phdr.p_vaddr < MEM_TASK_BASE) {
      continue;
    }

    // 加载该程序段
    int err = load_phdr(file, &elf_phdr, page_dir);
    if (err < 0) {
      log_printf("load program failed\n");
      goto load_failed;
    }

    // 更新堆空间的位置，紧靠最后一个可加载段
    task->heap_start = elf_phdr.p_vaddr + elf_phdr.p_memsz;
    task->heap_end = task->heap_start;
  }

  // 成功解析并加载完整个elf文件后关闭文件，并返回程序入口地址
  sys_close(file);
  return elf_hdr.e_entry;

// 错误处理
load_failed:
  if (file >= 0) {  // 文件已被打开，则关闭该文件
    sys_close(file);
  }
  return 0;
}

/**
 * @brief 在新任务的参数拷贝到其用户栈顶的上方
 *
 * @param to_page_dir 新任务的页目录表
 * @param stack_top 新任务的栈顶地址
 * @param argv 参数的字符串数组
 * @param argc 参数的个数
 * @return int
 */
static int copy_args(uint32_t to_page_dir, char *stack_top, char *const *argv,
                     int argc) {
  task_args_t task_args;
  task_args.argc = argc;

  // 1.获取char*数组在目标虚拟空间中的的虚拟地址
  // char*数组将被拷贝到task_args的上方,所以直接让argv指向栈顶偏移task_args的大小个字节的空间即可
  // 即指向了该字符串数组第一个char*的地址
  task_args.argv = (char **)(stack_top + sizeof(task_args_t));

  // 2.获取char*数组对应的虚拟空间关联的物理地址
  char **dest_argv_tb =
      (char **)memory_get_paddr(to_page_dir, (uint32_t)task_args.argv);

  // 3.获取参数的存储地址, 偏移量为 task_args 加上
  // argc个参数的字符串指针的大小
  //TODO:多给一个空指针位置，不然在解析参数的时候没有结束标志可能会访问异常
  char *dest_arg = stack_top + sizeof(task_args_t) + sizeof(char *) * (argc + 1);

  // 3.将参数拷贝到dest_arg处，并将每个参数的地址记录到task.argv指向的char*数组中
  for (int i = 0; i < argc; ++i) {
    char *from = argv[i];
    int len = kernel_strlen(from) + 1;
    //将每个字符串的内容陆续拷贝到dest_arg处，即task_arg以及指针数组的紧邻上方
    int err = memory_copy_uvm_data((uint32_t)dest_arg, to_page_dir,
                                   (uint32_t)from, len);
    ASSERT(err >= 0);
    dest_argv_tb[i] = dest_arg;
    dest_arg += len;
  }

  //将字符串指针数组的最后一项
  if (argc) {
    dest_argv_tb[argc] = (char *)0; 
  }

  // 4.将task_args拷贝到用户虚拟空间中,紧邻栈顶上方，作为入口函数的参数
  return memory_copy_uvm_data((uint32_t)stack_top, to_page_dir, (uint32_t)&task_args,
                       sizeof(task_args_t));
}

/**
 * @brief execve系统调用，加载外部程序
 *
 * @param name 程序名
 * @param argv 命令行参数数组
 * @param env 程序继承的环境变量数组
 * @return int
 */
int sys_execve(char *name, char *const *argv, char *const *env) {
  // 1.获取当前任务进程
  task_t *task = task_current();

  // 2.获取当前任务的页目录表
  uint32_t old_page_dir = task->tss.cr3;

  // 3.创建一个新的页目录表
  uint32_t new_page_dir = memory_creat_uvm();
  if (new_page_dir == 0)  // 创建失败
    goto exec_failed;

  // 4.加载elf文件，替换当前任务
  uint32_t entry = load_elf_file(task, name, new_page_dir);
  if (entry == 0) goto exec_failed;

  // 5.为新进程分配用户栈空间
  uint32_t stack_top = MEM_TASK_STACK_TOP - MEM_TASK_ARG_SIZE;
  int err = memory_alloc_for_page_dir(
      new_page_dir, MEM_TASK_STACK_TOP - MEM_TASK_STACK_SIZE,
      MEM_TASK_STACK_SIZE, PTE_P | PTE_U | PTE_W);

  if (err < 0) goto exec_failed;

  // 6.将被执行任务的入口参数拷贝到栈上方对应内存空间
  int argc = strings_count(argv);
  err = copy_args(new_page_dir, (char *)stack_top, argv, argc);
  if (err < 0) goto exec_failed;

  // 7.获取系统调用的栈帧,因为每次通过调用门进入内核栈中都只会一帧该结构体的数据，
  // 所以用最高地址减去大小即可获得该帧的起始地址
  syscall_frame_t *frame =
      (syscall_frame_t *)(task_current()->tss.esp0 - sizeof(syscall_frame_t));

  // 8.更改进程用户栈的位置，并更改调用门返回后执行的指令地址为程序入口地址
  frame->esp = stack_top - 5 * 4;
  frame->eip = entry;

  // 9.让进程更清爽的运行，清空通用寄存器的值
  frame->eax = frame->ebx = frame->ecx = frame->edx = 0;
  frame->esi = frame->edi = frame->ebp = 0;
  frame->eflags = EFLAGS_IF | EFLAGS_DEFAULT_1;

  // 10.修改当前任务名为被执行任务名
  kernel_strncpy(task->name, get_file_name(name), TASK_NAME_SIZE);

  // 11.记录并设置新页目录表，并销毁原页目录表的虚拟映射关系
  task->tss.cr3 = new_page_dir;
  mmu_set_page_dir(new_page_dir);
  memory_destroy_uvm(old_page_dir);
  return 0;

exec_failed:
  // 执行失败，释放资源并恢复到原进程状态
  if (new_page_dir) {
    task->tss.cr3 = old_page_dir;
    mmu_set_page_dir(old_page_dir);
    memory_destroy_uvm(new_page_dir);
  }
  return -1;
}

/**
 * @brief 任务进程主动退出
 *
 */
void sys_exit(int status) {
  // 1.获取当前任务
  task_t *curr_task = task_current();

  // 2.关闭当前任务打开的文件
  for (int fd = 0; fd < TASK_OFILE_SIZE; ++fd) {
    file_t *file = curr_task->file_table[fd];
    if (file) {
      sys_close(fd);
      curr_task->file_table[fd] = (file_t *)0;
    }
  }

  //3.将该进程的子进程的父进程设为first_task，由其进行统一回收
  int move_child = 0; //标志位，判断是否当前进程已有子进程进入僵尸态
  //TODO:加锁
  mutex_lock(&task_table_lock);
  for (int i = 0; i < TASK_COUNT; ++i) {
    task_t *task = task_table + i;
    if (task->parent == curr_task) {
      task->parent = &task_manager.first_task;
      if (task->state == TASK_ZOMBIE) { //已有子进程提前退出进入僵尸态，则设置标志位
        move_child = 1;
      }
    }
  }
  //TODO:解锁
  mutex_unlock(&task_table_lock);
 

  // TODO:加锁
  idt_state_t state = idt_enter_protection();


  // 4.获取父进程，判断父进程是否在等待回收子进程资源
  task_t *parent = (task_t *)curr_task->parent;
  
  if (move_child && (parent != &task_manager.first_task)) {  
    //当前进程的父进程不是first_task,
    //需要对first_task进行唤醒，以使first_task
    //对当前进程的提前死亡的子进程进行资源回收
    if (task_manager.first_task.state == TASK_WAITTING) {
      task_set_ready(&task_manager.first_task);
    }

  }
  

  if (parent->state ==
      TASK_WAITTING) {  // 父进程处于阻塞并等待回收子进程资源的状态，需要唤醒父进程
    task_set_ready(parent);
    parent->state = TASK_READY;
  }

  // 3.设置进程状态标志为僵尸态并保存状态值
  curr_task->state = TASK_ZOMBIE;
  curr_task->status = status;

  // 5.将任务进程从就绪队列中取下
  task_set_unready(curr_task);

  // 6.切换任务进程
  task_switch();

  // TODO:解锁
  idt_leave_protection(state);
}

/**
 * @brief 回收进程资源
 *
 * @param status 传入参数，记录被回收的进程状态值
 * @return int  被回收的进程的pid
 */
int sys_wait(int *status) {
  // 1.获取当前进程
  task_t *curr_task = task_current();

  for (;;) {
    // TODO:加锁
    mutex_lock(&task_table_lock);

    // 2.遍历任务表,寻找子进程
    for (int i = 0; i < TASK_COUNT; ++i) {
      task_t *task = task_table + i;
      if (task->pid == 0 || task->parent != curr_task) {
        continue;
      }
      // 3.找到一个子进程，判断是否为僵尸态
      if (task->state == TASK_ZOMBIE) {  // 僵尸态，进行资源回收
        int pid = task->pid;
        *status = task->status;

        //释放任务
        task_uninit(task);

        // TODO:解锁
        mutex_unlock(&task_table_lock);

        // 3.4返回该进程的pid
        return pid;
      }
    }

    // TODO:解锁
    mutex_unlock(&task_table_lock);

    // 4.未找到僵尸态的子进程，则当前进程进入阻塞状态
    // TODO:加锁
    idt_state_t state = idt_enter_protection();

    task_set_unready(curr_task);
    curr_task->state = TASK_WAITTING;
    task_switch();

    // TODO:解锁
    idt_leave_protection(state);
  }
  return 0;
}