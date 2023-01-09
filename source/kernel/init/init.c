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
    //1.对cpu相关属性进行初始化
    cpu_init();

    
    for (;;){}
}