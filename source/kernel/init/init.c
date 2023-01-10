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
#include "cpu/cpu.h"

/**
 * @brief  对内核进行初始化操作
 * 
 */
void kernel_init(_boot_info_t_* boot_info) {
    //1.完成cpu相关的初始化，初始化并重新加载GDT表
    cpu_init();

    //2.重新加载GDT表之后再初始化一遍cpu各个寄存器
    

    
    for (;;){}
}