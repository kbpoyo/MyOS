/**
 * @file log.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义打印日志相关函数
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/log.h"

/**
 * @brief  初始化串行端口寄存器COM1
 * 
 */
void log_init(void) {
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x80);
    outb(COM1_PORT + 0, 0x3);
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0xc7);
    outb(COM1_PORT + 4, 0x0f);
}

void log_printf(const char *formate, ...) {
    const char *p = formate;

    //1.将字符串输出到串口
    while (*p != '\0') {
        //2.判断串口是否正在忙碌，是则阻塞等待
        while ((inb(COM1_PORT + 5) & (1 << 6)) == 0);
        
        outb(COM1_PORT, *(p++));
    }
    
    //3.换行
    outb(COM1_PORT, '\r');
    outb(COM1_PORT, '\n');
}