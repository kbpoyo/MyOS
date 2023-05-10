/**
 * @file os_cfg.h
 * @author kbpoyo (kbpoyo.com)
 * @brief 定义与操作系统有关的数据
 * @version 0.1
 * @date 2023-01-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef OS_CFG_H
#define OS_CFG_H

//内核代码段的选择子,gdt_table[1]
#define KERNEL_SELECTOR_CS (0x0001 << 3)

//内核数据段的选择子,gdt_table[2]
#define KERNEL_SELECTOR_DS (0x0002 << 3)

//系统调用门描述符的选择子
#define SYSCALL_SELECTOR    (0x0003 << 3)

//内核栈空间的大小为8kb
#define KERNEL_STACK_SIZE (8 * 1024)

//确定定时器多长时间发出一次中断,单位为ms
#define OS_TICKS_MS 10

//定义操作系统版本
#define OS_VERSION "1.0.0"

//静态分配任务，定义任务数量
#define TASK_COUNT 128

#endif