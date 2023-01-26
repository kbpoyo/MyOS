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


//汇编中定义宏的指令, name为异常函数名，num为中断向量表的下标，with_err_code标志是否压入了错误码
//使用 \宏 来指定预处理时宏替换的位置
.macro exception_handler name num with_err_code

//定义中断处理程序的实现
    .text
    .extern do_handler_\name
    .global exception_handler_\name

exception_handler_\name:
    //调用c处理函数前先保存现场,中断发生前cpu会自动在栈中压入 EFLAGS，CS， EIP 寄存器，以及错误代码
    
    //1.判断是否有错误代码被压入，如果没有则压入0来占位，并压入中断在向量表中的下标
    .if \with_err_code == 0
        push $0
    .endif
    push $\num

    //2.用 pusha 指令压入8个通用寄存器
    pusha

    //3.压入其他段寄存器
    push %ds
    push %es
    push %fs
    push %gs

    //4.将此时的esp的值(此时栈中 gs 的地址)压入栈中作为c处理函数的参数，即exception_frame_t结构体的起始地址指向 ->[gs]
    //使该结构体可以访问到之前保留现场所压入的所有信息，这些信息可以看作一个栈帧
    push %esp

    //5.调用c处理函数，会自动压入返回地址
    call do_handler_\name

    //6.将 esp 重新指向 ->[gs]
    pop %esp

    //7.恢复现场
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $(2*4), %esp //弹出num 和 err_code,cpu恢复现场时自己压入的错误码不会自己弹出，而是直接pop给EIP，所以要手动弹出

    iret //中断处理程序必须要用iret进行返回，所以必须用汇编来实现

//宏结束标志
.endm

//使用定义的异常宏 
//                |异常类型|       |IDT索引| |错误码|
exception_handler unknown,              -1, 0    //未知异常
exception_handler divider,              0,  0    //除0异常
exception_handler debug,                1,  0    //debug异常
exception_handler nmi,                  2,  0
exception_handler breakpoint,           3,  0
exception_handler overflow,             4,  0
exception_handler bound_range,          5,  0
exception_handler invalid_opcode,       6,  0
exception_handler device_unavailable,   7,  0
exception_handler double_fault,         8,  1
exception_handler invalid_tss,          10, 1
exception_handler segment_not_present,  11, 1
exception_handler stack_segment_fault,  12, 1
exception_handler general_protection,   13, 1
exception_handler page_fault,           14, 1
exception_handler fpu_error,            16, 0
exception_handler alignment_check,      17, 1
exception_handler machine_check,        18, 0
exception_handler smd_exception,        19, 0
exception_handler virtual_exception,    20, 0
exception_handler control_exception,    21, 1

//定时器的中断处理函数
exception_handler time,                 0x20, 0 

    .text
    .global simple_switch
simple_switch:  //simple_switch(from, to)
    //1.获取参数
    mov 8(%esp), %edx   //获取 to, 地址(指针) --> 目标任务的栈空间对应的 esp 的值
    mov 4(%esp), %eax   //获取 from, 地址(指针) --> 当前任务的栈空间对应的 esp 的值


    //2.被调用者保存当前任务的 ebp, ebx, esi, edi 到当前任务的栈中
    //eax, ecx, edx, eip, eflags 已由调用者压入当前任务的栈中
    push %ebp
    push %ebx
    push %esi
    push %edi

    //3.更新当前任务栈空间的 esp
    mov %esp, (%eax)

    //3.进行栈任务栈的切换
    mov (%edx), %esp


    //4.恢复切换后的任务状态，ebp, ebx, esi, edi 手动恢复
    //eax, ecx, edx, eip, eflags, 是在当前任务(c函数)调用simple_switch时已压入当前任务栈中，
    //调用 ret 后会自行弹出并恢复
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp

    //5.恢复切换后的任务执行流，相当于此时 esp ->[eip]， 即 pop %eip
    ret 