/**
 * @file init.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  对内核进行初始化
 * @version 0.1
 * @date 2023-01-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "init.h"
#include "common/boot_info.h"
#include "common/cpu_instr.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "tools/log.h"
#include "tools/assert.h"
#include "dev/time.h"
#include "core/task.h"
#include "os_cfg.h"
#include "tools/list.h"
#include "test/test.h"
#include "tools/klib.h"
#include  "ipc/sem.h"
#include "core/memory.h"

/**
 * @brief  对内核进行初始化操作
 * 
 */
void kernel_init(boot_info_t* boot_info) {
    //1.判断是否检测到可用内存
    ASSERT(boot_info->ram_region_count != 0);
    
    //2.初始化日志程序,便于后期调用
    log_init();

    //3.初始化内存管理
    memory_init(boot_info);  

    //4.初始化并重新加载GDT表
    gdt_init();

    //5.初始化并加载中断描述符表IDT
    idt_init();

    //6.初始化定时器的中断处理
    time_init();

    //7.初始化任务管理器
    task_manager_init();
    //初始化完成后将在汇编里重新加载内核代码段与数据段的选择子，并为内核程序分配栈空间

}



/**
 * @brief 跳转到第一个任务进程
 * 
 */
void move_to_first_task(void) {
    //1.获取当前任务
    task_t *curr = task_current();
    ASSERT(curr != 0);

    //2.获取当前任务的tss结构
    tss_t *tss = &(curr->tss);

    //3.用内联汇编进行间接跳转,需要 jmp * %寄存器 (从寄存器中给出地址为间接跳转,直接从值跳转为直接跳转)
    __asm__ __volatile__(
        "jmp * %[ip]"::[ip]"r"(tss->eip)
    );
}


void init_main(void) {
    
    log_printf("Kernel is running......");
    log_printf("Name: %s, Version: %s", "KbOS", OS_VERSION);

    //当前任务作为任务管理器启用时的第一个任务
    task_first_init();

    //跳转到第一个任务进程去运行
    move_to_first_task();
}