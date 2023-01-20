/**
 * @file task.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  
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

/**
 * @brief  初始化任务的tss段
 * 
 * @param task 需要初始化的任务对象
 * @param entry 任务入口地址
 * @param esp 任务所用的栈顶指针
 */
static void tss_init(task_t *task, uint32_t entry, uint32_t esp) {

    //1.将tss段的值置空
    kernel_memset(&task->tss, 0, sizeof(task->tss));

    //2.设置eip，即任务的起始地址
    task->tss.eip = entry;

    //3.根据任务的特权级设置任务所使用的栈空间
    task->tss.esp = task->tss.esp0 = esp;

    //4.平坦模型，初始化栈空间段寄存器
    task->tss.ss = task->tss.ss0 = KERNEL_SELECTOR_DS;

    //5. 平台模型，初始其余化段寄存器
    task->tss.es = task->tss.fs = task->tss.gs = task->tss.ds = KERNEL_SELECTOR_DS;

    //6.平坦模型，初始化代码段寄存器
    task->tss.cs = KERNEL_SELECTOR_CS;

    //7.初始化eflags寄存器，使cpu中断保持开启
    task->tss.eflags = EFLAGS_DEFAULT_1 + EFLAGS_IF;

    //8.将该TSS段绑定到GDT中的某个段描述符
    uint32_t tss_selector = gdt_alloc_desc();
    if (tss_selector < 0) {
        log_printf("alloc tss failed!");
        return;
    }

    segment_desc_set(tss_selector, (uint32_t)&task->tss, sizeof(task->tss), 
                    SEG_ATTR_P | SEG_ATTR_DPL_0 | SEG_ATTR_TYPE_TSS);

    //9.记录tss绑定到的描述符的选择子
    task->tss_selector = tss_selector;
}

/**
 * @brief  初始化任务
 * 
 * @param task 任务对象
 * @param entry 任务的入口地址
 * @param esp 任务指行时所用的栈顶指针
 * @return int 
 */
int task_init(task_t *task, uint32_t entry, uint32_t esp) {
    ASSERT(task != (task_t*)0);
    tss_init(task, entry, esp);
    return 0;    
}


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
    switch_to_tss(to->tss_selector);
}

