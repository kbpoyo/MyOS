/**
 * @file first_task.S
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 第一个任务进程，实现与操作系统的代码隔离
 * @version 0.1
 * @date 2023-04-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

    .code32 //生成32位模式下的指令
	.text
	.global first_task_entry
    .extern first_main

//第一个任务进程入口函数
first_task_entry:
    //操作系统将为该进程分配栈空间和代码段，即ss和cs寄存器，这里直接用ss寄存器初始化其余段寄存器即可
	mov %ss, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    //因为cs寄存器已由操作系统分配，所以直接跳转即可
	jmp first_main