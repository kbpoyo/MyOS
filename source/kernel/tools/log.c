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
#include <stdarg.h> //包含一些处理可变参数的宏
#include "common/types.h"
#include "common/cpu_instr.h"
#include "tools/klib.h"
#include "cpu/idt.h"
#include "ipc/mutex.h"

//定义互斥锁保护输出资源，确保输出为原子操作
static mutex_t mutex;

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

    //初始化互斥锁
    mutex_init(&mutex);
}

/**
 * @brief  格式化输出到串口
 * 
 * @param formate 
 * @param ... 
 */
void log_printf(const char *formate, ...) {
    //1.设置字符缓冲区
    char str_buf[128];
    kernel_memset(str_buf, '\0', sizeof(str_buf));
    
    //2.获取可变参数并将其格式化到缓冲区中
    va_list args;
    va_start(args, formate);
    kernel_vsprintf(str_buf, formate, args);
    va_end(args);

    const char *p = str_buf;

    //将以下资源放入临界资源包含区，防止在运行时发生进程切换（cpu关中断）
    mutex_lock(&mutex); //TODO:加锁

    //3.将字符串输出到串口
    while (*p != '\0') {
        //4.判断串口是否正在忙碌，是则阻塞等待
        while ((inb(COM1_PORT + 5) & (1 << 6)) == 0);

        if (*p == '\n') outb(COM1_PORT, '\r');
        
        outb(COM1_PORT, *(p++));
    }
    
    //5.换行
    outb(COM1_PORT, '\r');
    outb(COM1_PORT, '\n');

    //执行完毕，将资源离开临界资源保护区，(cpu开中断)
    mutex_unlock(&mutex); //TODO:解锁
}