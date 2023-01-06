/**
 * @file loader.h
 * @author kbpoyo (kbpoyo.com)
 * @brief   系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 			该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 			之后，将由BIOS跳转至0x7c00处开始运行
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 */

  	.code16					//生成16位模式下的指令
 	.text					//指定之后的内容是属于代码段
	.global _start			//声明全局变量，允其他文件使用符号_start(即地址)
	.extern loader_entry 	//告诉编译器loader_entry函数在其他文件中

_start:
	jmp loader_entry 

	.code32 //生成32位模式下的指令
	.text
	.global protect_mode_entry
	.extern loader_kernel //32位loader程序入口

//保护模式的入口函数
protect_mode_entry:
	//cs寄存器已进入32位模式，还需要将其他段寄存器全部设置为32位模式
	mov $16, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	//跳转到32位的loader程序中
	//TODO: $8, $地址 的传参模式之后会讲解
	jmp $8, $loader_kernel
