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

	#include "boot.h"

  	.code16				//16位代码，务必加上
 	.text
	.global _start		//链接的时候作为程序的入口地址
	.extern boot_entry 	//告诉编译器boot_entry函数在其他文件中

// bios自动将磁盘0磁头0柱面1扇区加载到0x7c00，而此处的代码段将放在最前面，即_start加载后为0x7c00
//链接器在链接时需要用-e(或--entry)指明程序的入口地址或符号(函数或段名),若没有指明则默认为_start
_start:
	//初始化段寄存器为0,采用平坦模型访问内存，cs寄存器默认为0，其他其实也是默认为0
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov $_start, %esp 	//初始化栈顶指针寄存器
						//即将0x7c00前面一段空间(约30kb的空闲区域)用做程序运行的栈空间
						//此处用了esp,即16位模式下用了32位模式的资源，指令前缀为反转前缀66

	//int 0x10 软中断调用显卡驱动，
	mov $0xe, %ah 	
	mov $'\r', %al 	
	int $0x10 		

	mov $0xe, %ah 	
	mov $'\n', %al 	
	int $0x10 	

	mov $0xe, %ah 	//ah = 0xe 表示显示字符
	mov $'K', %al 	// al 缓冲区记录显示内容
	int $0x10 		//进入中断程序
	mov $'b', %al 	
	int $0x10 		
	mov $'p', %al 	
	int $0x10 		
	mov $'o', %al 	
	int $0x10 	
	mov $'y', %al 	
	int $0x10 		
	mov $'o', %al 	
	int $0x10 		
	mov $'O', %al 	
	int $0x10
	mov $'S', %al 	
	int $0x10


	mov $0xe, %ah 	
	mov $'\r', %al 	
	int $0x10 		

	mov $0xe, %ah 	
	mov $'\n', %al 	
	int $0x10 		




//int 0x13 软中断调用磁盘驱动读取磁盘
read_loader:
	mov $0x8000, %bx	// es:bx = 加载到内存中的地址,此时es = 0， 即直接将读取的内容加载到内存0x8000处 
	mov $0x2, %ah 		// ah = 0x2(读扇区), ah = 0x3(写扇区)
	mov $64, %al		// al = 扇区数, 读取64个扇区, 即32kb
	mov $0x2, %cx 		// ch = 柱面，cl = 扇区编号， 在0柱面进行读取，bios从1开始为扇区编号，1扇区为mbr引导程序，所以从2扇区读取
	mov $0, %dh			// dh = 磁头，即盘面，当前从0盘面开始读取
	mov $0x80, %dl 		// dl = 驱动器，0x00:软盘A,0x01软盘B 
						//	0x80:硬盘A 0x81:硬盘B
	int $0x13 			//执行中断程序

	//出口参数, cf = 0表示操作成功否则失败，若成功则ah=0x00,al=传输的扇区数，否则ah=状态代码(参见功能号0x01)
	// cf为eflags寄存器的0号位，表示carry flag， 在运算过程中若最高位有进位或借位则置1
	jc read_loader		// jc/jb/jna 跳转条件为 cf = 1,即当前失败时跳转重新读取


	//加载完loader后跳转到loader入口程序执行
	jmp boot_entry

	//jmp .
	// 引导结束段, 将该扇区剩下的区域一直填充0，直到最后两个字节填充0x55 和 0xaa，bios以此为标记加载mbr
	.section boot_end
boot_sig: .byte 0x55, 0xaa
