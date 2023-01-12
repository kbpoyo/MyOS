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

    #include "os_cfg.h"    //gcc可以处理在汇编语言中包含c语言头文件(头文件中应该只能有宏定义)

    .text
    .extern kernel_init
    .extern init_main
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

    //完成内核初始化后，进行直接跳转，以替换cs寄存器中临时的内核代码段的选择子为此时内存中真正的内核代码段选择子
    //cs:KERNEL_SELECTOR_CS, ip:gdt_reload
    jmp $KERNEL_SELECTOR_CS, $gdt_reload    

gdt_reload:
    //替换其他段寄存器中临时的内核数据段的选择子为此时内存中真正的内核数据段的选择子
    mov $KERNEL_SELECTOR_DS, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    //重新指定栈空间位置，不再使用0~0x7c00这部分空间
    //stack段已在下面预留了KERNEL_STACK_SIZE的空间大小，直接将最高地址赋值给esp，esp便可使用这片空间
    mov $(stack + KERNEL_STACK_SIZE), %esp

    //继续初始化内核
    //TODO:后序再来说明该函数的作用
    jmp init_main 

//定义bss段，未初始化的数据段，不占据编译后的文件空间，只指定运行时的内存空间
    .bss
//comm命令支持其他程序访问该空间，lcommo不支持
//指定stack标号在运行时的内存空间大小为KERNEL_STACK_SIZE
.comm stack, KERNEL_STACK_SIZE 


//定义中断处理程序的实现
    .text
    .extern do_handler_unknown
    .global exception_handler_unknown
exception_handler_unknown:
    //调用c处理函数前先保存现场
    //1.中断发生前cpu会自动在栈中压入 EFLAGS，IP， CS 寄存器，以及错误代码
    //2.用 pusha 指令压入8个通用寄存器
    pusha
    //3.压入其他段寄存器
    push %ds
    push %es
    push %fs
    push %gs

    //4.调用c处理函数，会自动压入返回地址
    call do_handler_unknown

    //5.恢复现场
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    
    iret //中断处理程序必须要用iret进行返回，所以必须用汇编来实现