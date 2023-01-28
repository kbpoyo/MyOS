/**
 * @file task.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  用TSS实现任务切换效率低，所以自己实现任务调度算法
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
#include "cpu/gdt.h"


/**
 * @brief  初始化任务
 * 
 * @param task 任务对象
 * @param entry 任务的入口地址
 * @return int 
 */
int task_init(task_t *task, uint32_t entry, uint32_t esp) {
    ASSERT(task != (task_t*)0);
   
    //1.获取当前任务的栈顶指针
    uint32_t* pesp = (uint32_t*)esp;

    if (pesp) {
        //2.在simple_switch中，会调用ret指令来获取栈空间中已保留的eip值，从而进行跳转
        //在第一次进入任务时栈空间中未保存有对应值，所以手动压入任务入口地址
        *(--pesp) = entry;

        //3.由于进行任务切换时，会手动陆续弹出 edi esi ebx ebp
        //而第一次进入任务时，栈空间并为保存对应值，所以也要手动压入初始值
        //避免后续弹出时导致栈溢出
        *(--pesp) = 0;
        *(--pesp) = 0;
        *(--pesp) = 0;
        *(--pesp) = 0;

        //4.将当前esp值保存到任务中
        task->stack_esp = pesp;

    }
    return 0;    
}


/**
 * @brief  进行任务切换
 * 
 * @param from_stack_addr 地址 --> 当前任务的栈空间的esp值
 * @param to_stack_addr 地址 --> 目标任务的栈空间的esp值
 */
void simple_switch(uint32_t from_stack_addr, const uint32_t to_stack_addr);

/**
 * @brief  将任务从from切换到to
 * 
 * @param from 切换前的任务
 * @param to 切换后的任务
 */
void task_switch_from_to(task_t *from, task_t *to) {
    simple_switch((uint32_t)&from->stack_esp, (uint32_t)&to->stack_esp);
}

