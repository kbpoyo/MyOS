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
void kernel_init(_boot_info_t_* boot_info) {
    //1.初始化并重新加载GDT表
    gdt_init();

    //2.初始化并加载中断描述符表IDT
    idt_init();

}

void init_main(void) {
    int a = 3 / 0;
    for (;;){}
}