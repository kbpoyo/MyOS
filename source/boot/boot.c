/**
 * @file boot_info.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 *         boot扇区容量较小，仅512字节。由于dbr占用了不少字节，导致其没有多少空间放代码，
 *         所以功能只能最简化,并且要开启最大的优化-os
 * @versioinin0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */


__asm__(".code16gcc"); //生成16位模式下运行的程序(即地址位16位大小)，但大小限制在64kb

#include "boot.h" 

#define LOADER_START_ADDR 0x8000 //loader在内存中的地址


/**
 * Boot的C入口函数
 * 只完成一项功能，即从磁盘找到loader文件然后加载到内容中，并跳转过去
 */
void boot_entry(void) {
    ((void (*)(void))LOADER_START_ADDR)(); //用函数指针直接调用0x8000处的函数
} 

