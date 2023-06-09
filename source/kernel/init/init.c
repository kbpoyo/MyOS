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
#include "dev/console.h"
#include "dev/keyboard.h"

/**
 * @brief  对内核进行初始化操作
 * 
 */
void kernel_init(boot_info_t* boot_info) {
    //1.判断是否检测到可用内存
    ASSERT(boot_info->ram_region_count != 0);
    
    //2.初始化日志程序,便于后期调用
    log_init();

    //3.初始化控制台
    console_init();
    
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

    kbd_init();

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

    // //3.用内联汇编进行间接跳转,需要 jmp * %寄存器 (从寄存器中给出地址为间接跳转,直接从值跳转为直接跳转)
    // __asm__ __volatile__(
    //     "jmp * %[ip]"::[ip]"r"(tss->eip)
    // );

    //3.由于当前任务特权级仍为内核的最高特权级，为了切换到first_task时内将特权级切换到低特权级
    //需要模拟中断发生，因为只有iret指令和retf可以将cpu的特权级由高切换到低，同时也只有中断门和调用门中的调用发生才能
    //使cpu的特权级由低到高切换，且只切换ss,esp,eflags,cs,eip这几个寄存器都由在中断门调用发生时cpu自动压入
    //(ss,esp只在由低特权级触发中断门时压入，以用来iret恢复到原始特权级)
     __asm__ __volatile__(
        //模拟中断门调用时，cpu压入的特权级的寄存器,并用iret指令返回
        "push %[ss]\n\t"
        "push %[esp]\n\t"
        "push %[eflags]\n\t"
        "push %[cs]\n\t"
        "push %[eip]\n\t"
        "iret"::[ss]"r"(tss->ss), [esp]"r"(tss->esp), [eflags]"r"(tss->eflags), [cs]"r"(tss->cs), [eip]"r"(tss->eip)

        
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