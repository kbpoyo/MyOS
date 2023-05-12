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
#include "tools/klib.h"
#include "tools/assert.h"
#include "tools/log.h"
#include "common/cpu_instr.h"
#include "cpu/gdt.h"
#include "os_cfg.h"
#include "cpu/idt.h"
#include "core/memory.h"
#include "cpu/mmu.h"
#include "cpu/syscall.h"

//定义全局唯一的任务管理器对象
static task_manager_t task_manager;
//定义静态的任务表，用于任务的静态分配
static task_t task_table[TASK_COUNT];
//定义用于维护task_table的互斥锁
static mutex_t task_table_lock;


static void switch_to_tss(uint16_t tss_selector) {
    //进行远跳转，让cpu访问该tss段的描述符，cpu将重启到之前运行该tss对应的任务的状态继续运行该任务
    far_jump(tss_selector, 0);
}

/**
 * @brief  将任务从from切换到to
 * 
 * @param from 切换前的任务
 * @param to 切换后的任务
 */
void task_switch_from_to(task_t *from, task_t *to) {
    //跳转到对应的tss段读取并恢复cpu任务状态
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
   //1.将该TSS段绑定到GDT中的某个段描述符
    uint32_t tss_selector = gdt_alloc_desc();
    if (tss_selector < 0) {
        log_printf("alloc tss failed!");
        return -1;
    }
    segment_desc_set(tss_selector, (uint32_t)&task->tss, sizeof(task->tss), 
                    SEG_ATTR_P | SEG_ATTR_DPL_0 | SEG_ATTR_TYPE_TSS);


    //2.将tss段的值置空
    kernel_memset(&task->tss, 0, sizeof(task->tss));

    //3.记录tss绑定到的描述符的选择子
    task->tss_selector = tss_selector;


    //4.根据任务的特权级来设置对应选择子的cpl
    uint32_t code_selector, data_selector;
    if (flag & TASK_FLAGS_SYSTEM) { //内核特权级
        code_selector = KERNEL_SELECTOR_CS;
        data_selector = KERNEL_SELECTOR_DS;
    } else {    //用户特权级
        //获取任务的代码段选择子和数据段选择子，并初始化其CPL,因为是平台模型，所以基本不涉及到跨段访问，所以不需要RPL
        code_selector = task_manager.app_code_selector | SEG_CPL3;
        data_selector = task_manager.app_data_selector | SEG_CPL3;
    }


    //5.设置eip，即任务的起始地址
    task->tss.eip = entry;

    //6.根据任务的特权级设置任务所使用的栈空间
    task->tss.esp =  esp;   //特权级为3的栈空间
    uint32_t kernel_stack = memory_alloc_page();  //分配一页作为内核特权级0的栈空间
    if (kernel_stack == 0) //内核栈空间初始化失败
        goto tss_init_failed;   
    else task->tss.esp0 =  kernel_stack + MEM_PAGE_SIZE;//特权级为0的栈空间

    //7.平坦模型，初始化栈空间段寄存器
    task->tss.ss =  data_selector;  //特权级为3时使用的栈段
    task->tss.ss0 = KERNEL_SELECTOR_DS; //特权级为0时使用的栈段，
                                        //由于平坦模型，其实使用的是同一片空间，只是特权级发生了变化

    //8. 平坦模型，初始其余化段寄存器
    task->tss.es = task->tss.fs = task->tss.gs = task->tss.ds = data_selector;

    //9.平坦模型，初始化代码段寄存器
    task->tss.cs = code_selector;

    //10.初始化eflags寄存器，使cpu中断保持开启
    task->tss.eflags = EFLAGS_DEFAULT_1 | EFLAGS_IF;

    //11.创建当前进程的虚拟页目录表，并设置cr3寄存器
    uint32_t page_dir = memory_creat_uvm();
    if (page_dir == 0) 
        goto tss_init_failed;
    task->tss.cr3 = page_dir;
    
    return 0;

//tss初始化失败
tss_init_failed:
    gdt_free(tss_selector);  //释放选择子

    if (kernel_stack) { //内核栈空间分配有效，需要释放
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
    idt_state_t state = idt_enter_protection();//TODO:加锁
   
    //5.将任务加入任务队列
    list_insert_last(&task_manager.task_list, &task->task_node);

    //6.将任务设置为就绪态
    task_set_ready(task);

    
    idt_leave_protection(state);//TODO:解锁
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
int task_init(task_t *task, const char* name, uint32_t entry, uint32_t esp, uint32_t flag) {
    ASSERT(task != (task_t*)0);
    //1.初始化任务TSS段
    int err = tss_init(task, entry, esp, flag);
    if (err == -1)
        return err;
    

    //2.初始化任务名称
    kernel_strncpy(task->name, name, TASK_NAME_SIZE);

    //3.初始化任务队列节点及就绪队列节点
    list_node_init(&task->ready_node);
    list_node_init(&task->task_node);
    list_node_init(&task->wait_node);

    //4.初始化最大时间片数与当前拥有时间片数,以及延时时间片数
    task->state = TASK_CREATED;
    task->slice_max = task->slice_curr = TASK_TIME_SLICE_DEFAULT;
    task->sleep = 0;
    task->pid = (uint32_t)task;
    task->parent = (task_t*)0;
   

    return 1;
}

/**
 * @brief 反初始化任务对象，释放对应的资源
 * 
 * @param task 
 */
void task_uninit(task_t *task) {
    if (task->tss_selector) {   //释放已分配的选择子
        gdt_free(task->tss_selector);
    }

    if (task->tss.esp0) {   //释放已分配的内核栈空间
        memory_free_page((uint32_t)(task->tss.esp0 - MEM_PAGE_SIZE));
    }

    if (task->tss.cr3) {    //释放为页目录分配的页空间及其映射关系
        memory_destroy_uvm(task->tss.cr3);
    }

    kernel_memset(task, 0, sizeof(task_t));
}


//空闲进程的栈空间
static uint32_t empty_task_stack[EMPTY_TASK_STACK_SIZE];
/**
 * @brief  空闲进程，当所有进程都延时运行时，让cpu运行空闲进程
 * 
 */
static void empty_task(void) {

    while(1) {
        //停止cpu运行，让cpu等待时间中断
        hlt();
    };

}


/**
 * @brief  初始化任务管理器
 * 
 */
void task_manager_init(void) {
    //1.初始化应用程序及所有任务的代码段选择子和数据段选择子，以区分和内核选择子的特权级
    //应用程序运行在 DPL_3 下， 内核运行在DPL_0下，配合分页机制中的us位，0为内核特权级，1为用户特权级
    //就可做到特权级保护
    uint32_t data_selector = gdt_alloc_desc();
    uint32_t code_selector = gdt_alloc_desc();
    segment_desc_set(data_selector, 0, 0xffffffff, SEG_ATTR_P | SEG_ATTR_DPL_3 | SEG_ATTR_S_NORMAL | SEG_ATTR_TYPE_DATA | SEG_ATTR_TYPE_RW | SEG_ATTR_D_OR_B);
    segment_desc_set(code_selector, 0, 0xffffffff, SEG_ATTR_P | SEG_ATTR_DPL_3 | SEG_ATTR_S_NORMAL | SEG_ATTR_TYPE_CODE | SEG_ATTR_TYPE_RW | SEG_ATTR_D_OR_B);
    task_manager.app_code_selector = code_selector;
    task_manager.app_data_selector = data_selector;
    
    //2.初始化所有任务队列
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    list_init(&task_manager.sleep_list);

    //3.将当前任务置零
    task_manager.curr_task = (task_t*)0;

    //4.初始化空闲进程
    task_init(  &task_manager.empty_task,
                 "empty_task", 
                 (uint32_t)empty_task,
                 (uint32_t)&empty_task_stack[EMPTY_TASK_STACK_SIZE], TASK_FLAGS_SYSTEM);
                 


    //5.初始化静态任务表,及其互斥锁
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

    //1.声明第一个任务的符号
    void first_task_entry(void);

    //2.确定第一个任务进程需要的空间大小
    extern char s_first_task, e_first_task;
    uint32_t copy_size = (uint32_t)(&e_first_task - &s_first_task);   //进程所需空间大小
    uint32_t alloc_size = up2(copy_size, MEM_PAGE_SIZE) + 10 * MEM_PAGE_SIZE;   //需要为进程分配的内存大小，按4kb对齐,并多拿五页当作栈空间
    ASSERT(copy_size < alloc_size);

    uint32_t task_start_addr = (uint32_t)first_task_entry;  //获取第一个任务的入口地址

    //3.初始化第一个任务,因为当前为操作系统进程，esp初始值随意赋值都可，
    // 因为当前进程已开启，cpu会在切换的时候保留真实的状态，即真实的esp值
    task_init(&task_manager.first_task, "first task", task_start_addr, task_start_addr + alloc_size, TASK_FLAGS_USER);
      
    //4.将当前任务的TSS选择子告诉cpu，用来切换任务时保存状态
    write_tr(task_manager.first_task.tss_selector);

    //5.将当前任务执行第一个任务
    task_manager.curr_task = &task_manager.first_task;

    //6.将当前页表设置为第一个任务的页表
    mmu_set_page_dir(task_manager.first_task.tss.cr3);

    //7.将当前任务状态设置为运行态
    task_manager.curr_task->state = TASK_RUNNING;

    //8.进程的各个段还只是在虚拟地址中，所以要为各个段分配物理地址页空间，并进行映射
    memory_alloc_page_for(task_start_addr, alloc_size, PTE_P | PTE_W | PTE_U);

    //9.将任务进程各个段从内核四个段之后的紧邻位置，拷贝到已分配好的且与虚拟地址对应的物理地址空间，实现代码隔离
    kernel_memcpy(first_task_entry, &s_first_task, alloc_size);

    //10.将任务设为可被调度
    task_start(&task_manager.first_task);
}

/**
 * @brief  获取当前任务管理器的第一个任务
 * 
 * @return task_t* 
 */
task_t *task_first_task(void) {
    return &task_manager.first_task;
}

/**
 * @brief  将任务task加入就绪队列
 * 
 * @param task 需要加入就绪队列的任务
 */
void task_set_ready(task_t *task) {
    ASSERT(task != (task_t*)0);
    //1.将任务插入到就绪队列的尾部
    list_insert_last(&task_manager.ready_list, &task->ready_node);

    //2.将任务状态设置为就绪态
    task->state = TASK_READY;
}


/**
 * @brief  将任务task从就绪队列中取下
 * 
 * @param task 
 */
void task_set_unready(task_t *task) {
    ASSERT(task != (task_t*)0);
    list_remove(&task_manager.ready_list, &task->ready_node);
    task->state = TASK_CREATED;
}

/**
 * @brief  获取就绪队列中的第一个任务
 * 
 */
task_t* task_ready_first(void) {
    list_node_t *ready_node = list_get_first(&task_manager.ready_list);
    
    return list_node_parent(ready_node, task_t, ready_node);
}

/**
 * @brief  获取当前正在运行的任务
 * 
 * @return task_t* 
 */
task_t *task_current(void) {
    return task_manager.curr_task;
}



/**
 * @brief  任务管理器进行任务切换
 * 
 */
void task_switch(void) {

    idt_state_t state = idt_enter_protection();//TODO:加锁

    //1.获取就绪队列中的第一个任务
    task_t *to = task_ready_first();

    //2.若获取到的任务不是当前任务就进行切换
    if (to != task_manager.curr_task) {
        //3.获取当前任务
        task_t *from = task_manager.curr_task;

        //4.目标任务若为空，则所有任务都在延时，让cpu运行空闲任务
        if (to == (task_t*)0) { 
            to = &task_manager.empty_task;
        }
        //5.切换当前任务, 并将当前任务置为运行态
        to->state = TASK_RUNNING;
        task_manager.curr_task = to;
        

        //6.进行任务切换
        task_switch_from_to(from, to);
    } 

    idt_leave_protection(state);//TODO:解锁
}





/**
 * @brief  提供给时钟中断使用，每中断一次，当前任务的时间片使用完一次
 *         减少当前任务的时间片数，并判断是否还有剩余时间片，若没有就进行任务切换
 * 
 */
void task_slice_end(void) {

    //1.遍历当前延时队列，判断是否有可唤醒的任务
    list_node_t *curr_sleep_node = list_get_first(&task_manager.sleep_list);

    //2.遍历并判断每一个任务执行完当前时间片是否可被唤醒，若可以则唤醒
    while (curr_sleep_node) {
        
        list_node_t *next_sleep_node = list_node_next(curr_sleep_node);
        
        task_t *curr_sleep_task = list_node_parent(curr_sleep_node, task_t, ready_node);
        if (--curr_sleep_task->sleep == 0) {
            task_set_wakeup(curr_sleep_task);   //从延时队列中取下
            task_set_ready(curr_sleep_task);    //加入就绪队列
        }

        curr_sleep_node = next_sleep_node;
    }

    // task_switch(); 没有必要立马进行任务切换，当前任务时间片用完后会自动切换
    //3.获取当前任务
    task_t *curr_task = task_current();

    //4.若当前任务为空闲任务，则判断就绪队列是否为空
    if (curr_task == &task_manager.empty_task) {
        
        if (list_is_empty(&task_manager.ready_list)) return;
        
        task_manager.empty_task.state = TASK_CREATED;
        
        task_switch();//就绪队列有任务，则直接切换任务
    }

    //5.若当前任务为普通任务则，减小当前时间片数
    if (curr_task != &task_manager.empty_task && --curr_task->slice_curr == 0) {
        //6.时间片数用完了，重置时间片并进行任务切换
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
    ASSERT(task != (task_t*)0);
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
    ASSERT(task != (task_t*)0);
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

    mutex_lock(&task_table_lock);
    
    //遍历静态任务表，取出未被分配的任务对象空间
    for (int i = 0; i < TASK_COUNT; ++i) {
        task_t *curr = task_table + i;
        if (curr->pid == 0) {
            task = curr;
            break;
        }
    }

    mutex_unlock(&task_table_lock);

    return task;
}

/**
 * @brief 释放静态任务表的任务对象
 * 
 * @param task 
 */
static void free_task(task_t *task) {
    mutex_lock(&task_table_lock);
    
    task->pid = 0;

    mutex_unlock(&task_table_lock);
}


/**
 * @brief  使进程进入延时状态
 * 
 * @param ms 延时的时间，以ms为单位
 */
void sys_sleep(uint32_t ms) {
    idt_state_t state = idt_enter_protection(); //TODO:加锁

    //1.获取当前任务  
    task_t *curr_task = task_current();
    
    //2.将当前任务离开就绪队列
    task_set_unready(curr_task);

    //3.计算出需要延时的时间片数，对时间片数向上取整，保证进程至少能延时指定时间
    uint32_t slice = (ms + (OS_TICKS_MS - 1)) / OS_TICKS_MS;

    //4.将当前任务放入延时队列，并设置延时时间片数
    task_set_sleep(curr_task, slice);

    //5.切换任务
    task_switch();

    idt_leave_protection(state); //TODO:解锁
}

/**
 * @brief 获取任务pid
 * 
 * @return int pid
 */
int sys_getpid(void) {
    return task_current()->pid;
}

/**
 * @brief 创建子进程
 * 
 * @return int 子进程的pid
 */
int sys_fork(void) {

    //获取当前进程为fork进程的父进程
    task_t *parent_task = task_current();

    //分配子进程控制块
    task_t *child_task = alloc_task();
    if (child_task == (task_t*)0)
        goto fork_failed;

    //获取系统调用的栈帧,因为每次通过调用门进入内核栈中都只会一帧该结构体的数据，
    //所以用最高地址减去大小即可获得该帧的起始地址
    syscall_frame_t *frame = (syscall_frame_t*)(parent_task->tss.esp0 - sizeof(syscall_frame_t));

    //初始子进程控制块，直接用父进程进入调用门的下一条指令地址作为子进程的入口地址
    int err = task_init(child_task, parent_task->name, frame->eip, frame->esp + sizeof(uint32_t)*SYSCALL_PARAM_COUNT, TASK_FLAGS_USER);
    if (err < 0)
        goto fork_failed;

    //恢复到父进程的上下文环境
    tss_t *tss = &(child_task->tss);
    //子进程执行的第一条指令就是从eax中取出系统用的返回值，即进程id，子进程固定获取0
    tss->eax = 0;   
    tss->ebx = frame->ebx;
    tss->ecx = frame->ecx;
    tss->edx = frame->edx;
    tss->edi = frame->edi;
    tss->esi = frame->esi;
    tss->eflags = frame->eflags;
    tss->ebp = frame->ebp;

    //复用父进程的段寄存器
    tss->cs = frame->cs;
    tss->ds = frame->ds;
    tss->es = frame->es;
    tss->fs = frame->fs;
    tss->gs = frame->gs;
    tss->ss = frame->ss;

    //记录父进程地址
    child_task->parent = parent_task;

    //拷贝进程虚拟页目录表和页表，即拷贝其映射关系
    if (memory_copy_uvm(tss->cr3, parent_task->tss.cr3) < 0)
        goto fork_failed;

    //子进程控制块初始化完毕，设为可被调度态
    task_start(child_task);
    //反回子进程id
    return child_task->pid;

//fork失败，清理资源
fork_failed:
    if (child_task) {   //初始化失败，释放对应资源
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
    idt_state_t state = idt_enter_protection();//TODO:加锁
    
    //1.判断当前就绪队列中是否有多个任务
    if (list_get_size(&task_manager.ready_list) > 1) {
        //2.获取当前任务  
        task_t *curr_task = task_current();
        
        //3.将当前任务从就绪队列中取下
        task_set_unready(curr_task);

        //4.将当前任务重新加入到就绪队列的队尾
        task_set_ready(curr_task);

        //5.任务管理器运行下一个任务，从而释放cpu使用权
        task_switch();
    }
    
    idt_leave_protection(state);//TODO:解锁
    return 0;
}

/**
 * @brief execve系统调用，加载外部程序
 * 
 * @param name 程序名
 * @param argv 命令行参数数组
 * @param env 程序继承的环境变量数组
 * @return int 
 */
int sys_execve(char *name, char * const *argv, char * const *env ) {

}