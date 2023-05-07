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
    //iret指令返回后，将之前压入的 ss,esp,eflags,cs,eip 寄存器值返回给对应寄存器，
	//由于cpu从0特权级切换到了3特权级，而之前其余的段寄存器都是0特权级，不能供3特权级使用，
	//所以cpu将其余寄存器置零，这里直接用ss寄存器再初始化其余段寄存器即可
	mov %ss, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    //因为cs寄存器已由操作系统分配，所以直接跳转即可
	jmp first_main