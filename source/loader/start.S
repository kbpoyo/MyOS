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
	.extern load_kernel //32位loader程序入口

//保护模式的入口函数
protect_mode_entry:
	//cs寄存器已进入32位模式，还需要将其他段寄存器全部设置为32位模式
	//已经进入了保护模式，所以cpu正在执行的16位模式的流水线失效
	//重置各个段寄存器，存储选择子，0x10即指向第3个段描述符(数据段)
	//第三个段描述符所指的段基址为0，所以继续平坦模式
	//且第三个段描述符的属性位的G位设置为1，标志了该段为32位程序，其他位也设置了正确值
	//临时的内核数据段描述符:{0xffff, 0x0000, 0x9200, 0x00cf}
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	//跳转到32位的loader程序中，jmp指令可以清空流水线
	jmp $8, $load_kernel //直接跳转 cs:8， ip:load_kernel, 8为1000b, 即选择此时GDT中的第2个段描述符(代码段)
