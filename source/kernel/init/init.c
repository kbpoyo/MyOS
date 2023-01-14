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


/**
 * @brief  对内核进行初始化操作
 * 
 */
void kernel_init(boot_info_t* boot_info) {
    //1.初始化并重新加载GDT表
    gdt_init();

    //2.初始化并加载中断描述符表IDT
    idt_init();

    //3.初始化定时器的中断处理
    time_init();





    //初始化完成后将在汇编里重新加载内核代码段与数据段的选择子，并为内核程序分配栈空间

}


void init_main(void) {
    idt_enable_global(); 
    for (;;){}
}