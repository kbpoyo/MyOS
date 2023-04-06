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

//定义全局唯一的任务管理器对象
static task_manager_t task_manager;


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
 */
static int tss_init(task_t *task, uint32_t entry, uint32_t esp) {

    //1.将tss段的值置空
    kernel_memset(&task->tss, 0, sizeof(task->tss));

    //2.设置eip，即任务的起始地址
    task->tss.eip = entry;

    //3.根据任务的特权级设置任务所使用的栈空间
    task->tss.esp = task->tss.esp0 = esp;

    //4.平坦模型，初始化栈空间段寄存器
    task->tss.ss = task->tss.ss0 = KERNEL_SELECTOR_DS;

    //5. 平坦模型，初始其余化段寄存器
    task->tss.es = task->tss.fs = task->tss.gs = task->tss.ds = KERNEL_SELECTOR_DS;

    //6.平坦模型，初始化代码段寄存器
    task->tss.cs = KERNEL_SELECTOR_CS;

    //7.初始化eflags寄存器，使cpu中断保持开启
    task->tss.eflags = EFLAGS_DEFAULT_1 | EFLAGS_IF;

    //8.创建当前进程的虚拟页表，并设置cr3寄存器
    uint32_t page_dir = memory_creat_uvm();
    if (page_dir == 0) return -1;
    task->tss.cr3 = page_dir;
    


    //9.将该TSS段绑定到GDT中的某个段描述符
    uint32_t tss_selector = gdt_alloc_desc();
    if (tss_selector < 0) {
        log_printf("alloc tss failed!");
        return -1;
    }
    segment_desc_set(tss_selector, (uint32_t)&task->tss, sizeof(task->tss), 
                    SEG_ATTR_P | SEG_ATTR_DPL_0 | SEG_ATTR_TYPE_TSS);


    //10.记录tss绑定到的描述符的选择子
    task->tss_selector = tss_selector;

    return 0;
}

/**
 * @brief  初始化任务
 * 
 * @param task 任务对象
 * @param entry 任务的入口地址
 * @param esp 任务指行时所用的栈顶指针
 * @return int 
 */
void task_init(task_t *task, const char* name, uint32_t entry, uint32_t esp) {
    ASSERT(task != (task_t*)0);
    //1.初始化任务TSS段
    tss_init(task, entry, esp);

    //2.初始化任务名称
    kernel_strncpy(task->name, name, TASK_NAME_SIZE);

    //3.初始化任务队列节点及就绪队列节点
    list_node_init(&task->ready_node);
    list_node_init(&task->task_node);
    list_node_init(&task->wait_node);

    //4.初始化最大时间片数与当前拥有时间片数,以及延时时间片数
    task->slice_max = task->slice_curr = TASK_TIME_SLICE_DEFAULT;
    task->sleep = 0;

    
    idt_state_t state = idt_enter_protection();//TODO:加锁
   
    //5.将任务加入任务队列
    list_insert_last(&task_manager.task_list, &task->task_node);

    //6.将任务设置为就绪态
    task_set_ready(task);

    
    idt_leave_protection(state);//TODO:解锁
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
    //1.初始化所有任务队列
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    list_init(&task_manager.sleep_list);

    //2.将当前任务置零
    task_manager.curr_task = (task_t*)0;

    //3.初始化空闲进程
    task_init(  &task_manager.empty_task,
                 "empty_task", 
                 (uint32_t)empty_task,
                 (uint32_t)&empty_task_stack[EMPTY_TASK_STACK_SIZE]);
                 

    //4.将空闲进程从就绪队列中取出
    task_set_unready(&task_manager.empty_task);
    task_manager.empty_task.state = TASK_CREATED;
}

/**
 * @brief  初始化第一个任务
 * 
 */
void task_first_init(void) {
      //1.初始化任务，当前任务是在任务管理器启用前就执行的，
      //拥有自己的栈空间，所以入口地址直接和栈空间都置0即可
      //这一步只是为当前任务绑定一个TSS段并将其绑定到一个task对象
      task_init(&task_manager.first_task, "first task", 0, 0);
      
      //2.将当前任务的TSS选择子告诉cpu，用来切换任务时保存状态
      write_tr(task_manager.first_task.tss_selector);

      //3.将当前任务执行第一个任务
      task_manager.curr_task = &task_manager.first_task;

      //4.将当前任务状态设置为运行态
      task_manager.curr_task->state = TASK_RUNNING;
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