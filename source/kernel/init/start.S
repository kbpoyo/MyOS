/**
 * @file kernel/init/start.S
 * @author kbpoyo (kbpoyo.com)
 * @brief  进行内核初始化
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

    .text
    .extern kernel_init
    .global _start
_start:
    call kernel_init    //调用内核初始化函数
    jmp .
