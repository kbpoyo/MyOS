/**
 * 自己动手写操作系统
 *
 * 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 之后，将由BIOS跳转至0x7c00处开始运行
 */
	#include "boot.h"

  	.code16				//16位代码，务必加上
 	.text
	.global _start		//链接的时候作为程序的入口地址
	.extern boot_entry 	//告诉编译器boot_entry函数在其他文件中

// bios自动将磁盘0磁头0柱面1扇区加载到0x7c00，而此处的代码段将放在最前面，即_start加载后为0x7c00
//链接器在链接时需要用-e(或--entry)指明程序的入口地址或符号(函数或段名),若没有指明则默认为_start
_start:
	//初始化段寄存器
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov $_start, %esp 	//初始化栈顶指针寄存器，即将0x7c00前面一段空间用做程序运行的栈空间
						//此处用了esp,即16位模式下用了32位模式的资源，指令前缀为反转前缀66

	//int 0x10 软中断调用显卡驱动，
	mov $0xe, %ah 	//ah = 0xe 表示显示字符
	mov $'S', %al 	// al 缓冲区记录显示内容
	int $0x10 		//进入中断程序

	mov $0xe, %ah 	
	mov $'\r', %al 	
	int $0x10 		

	mov $0xe, %ah 	
	mov $'\n', %al 	
	int $0x10 		




//int 0x13 软中断调用磁盘驱动读取磁盘
read_loader:
	mov $0x8000, %bx	// es:bx = 加载到内存中的地址,此时es = 0， 即直接将读取的内容加载到内存0x8000处 
	mov $0x2, %ah 		// ah = 0x2, al = 扇区数, 
	mov $64, %al		// 读取64个扇区, 即32kb
	mov $0x2, %cx 		// ch = 柱面，cl = 扇区编号， 在0柱面进行读取，bios从1开始为扇区编号，1扇区为mbr引导程序，所以从2扇区读取
	mov $0, %dh			// dh = 磁头，即盘面，当前从0盘面开始读取
	mov $0x80, %dl 		// dl = 驱动器，0x00~0x7f:软盘， 0x80~0xff:硬盘
	int $0x13 			//执行中断程序

	//出口参数, cf = 0表示操作成功否则失败，若成功则ah=0x00,al=传输的扇区数，否则ah=状态代码(参见功能号0x01)
	// cf为eflags寄存器的0号位，表示carry flag， 在运算过程中若最高位有进位或借位则置1
	jc read_loader		// jc/jb/jna 跳转条件为 cf = 1,即当前失败时跳转重新读取


	//加载完loader后跳转到loader入口程序执行
	jmp boot_entry

	//jmp .
	// 引导结束段, 将该扇区剩下的区域一直填充0，直到最后两个字节填充0x55 和 0xaa，bios以此为标记加载mbr
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
