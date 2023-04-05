/**
 * @file kernel.lds
 * @author kbpoyo (kbpoyo.com)
 * @brief  对链接程序时的各个段进行安排
 * @version 0.1
 * @date 2023-01-07
 * @copyright Copyright (c) 2023
 * 
 */


SECTIONS {/*SECTIONS必须大小*/

    /*让链接后的程序以此顺序布局各个段中(0x10000为.text的起始位置) */
    . = 0x10000;

    /*记录只读段的起始地址*/
    PROVIDE(s_text = .);
    /*链接时将所有源文件生成的目标文件的代码段都合并到一个代码段，*为通配符 */
     .text : {   /*冒号前必须有空格 */
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }
    /* 记录只读段的结束地址 */
    /* 段结束地址为真正结束位置的下一个字节 */
    PROVIDE(e_text = .);


    /* 记录可读写段的起始地址 */
    . = ALIGN(4096); /*让当前地址按4kb对齐*/
    PROVIDE(s_data = .);
    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    /*记录内核四个段后的紧邻的空间地址, 
    该变量只被链接器认为地址为当前地址 . 
    但并不占用内存，即并不会改变当前地址 . 的值*/
    PROVIDE(mem_free_start = .);
 }

