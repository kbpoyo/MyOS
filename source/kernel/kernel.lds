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

    /*链接时将所有源文件生成的目标文件的代码段都合并到一个代码段，*为通配符 */
     .text : {   /*冒号前必须有空格 */
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
 }

