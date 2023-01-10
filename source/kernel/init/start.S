/**
 * @file kernel/init/start.S
 * @author kbpoyo (kbpoyo.com)
 * @brief  进行内核初始化
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

    .text
    .extern kernel_init
    .extern KERNEL_SELECTOR_CS
    .extern KERNEL_SELECTOR_DS
    .global _start

/**
*   内核在内存中的入口地址，0x100 000
*   &boot_info 会先传递到这个地址
*   所以这里相当于一个函数 void _start(_boot_info_t_*),且该函数不会返回，所以只用在意参数传递即可
*    
*    函数调用流程：
*    1.在外部函数中从右到左push参数
*    2.最后执行call(会push返回地址)
*    3.调用的函数会先push ebp 来保存外部函数的ebp
*    4.将当前esp的值赋值给ebp
*    5.push自己的局部变量
*    5.用ebp进行偏移访问参数和外部函数的局部变量
*    
*    但这里因为不会调用ret来返回，所以不用保存ebp，也没有局部变量
*    所以当前的esp指向内存布局为：
*            [&boot_info]
*    esp ->  [返回地址]
*   所以要想访问到&boot_info，则只需从当前esp的位置偏移+4字节即可
*/
_start:
    mov %esp, %ebp      //栈空间最好只用ebp来进行偏移访问
    mov 0x4(%ebp), %eax //将&boot_info的值存入eax中

    push %eax           //外部函数将参数压入栈中，再进行函数调用
    call kernel_init    //调用内核初始化函数

    //初始化完内核后再将cpu各个寄存器重置到刚进入保护模式的状态
    jmp $KERNEL_SELECTOR_CS, $gdt_reload

gdt_reload:
    mov $KERNEL_SELECTOR_DS, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    jmp .
