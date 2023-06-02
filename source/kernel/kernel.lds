/**
 * @file kernel.lds
 * @author kbpoyo (kbpoyo.com)
 * @brief  对链接程序时的各个段进行安排
 * @version 0.1
 * @date 2023-01-07
 * @copyright Copyright (c) 2023
 * 
 */


SECTIONS {/*SECTIONS必须大写*/

    /*让链接后的程序以此顺序布局各个段中(0x10000为.text的起始位置) */
    /*此处设置的其实是elf文件段头信息中的p_vaddr, 因为没有指定p_paddr，所以p_paddr默认与p_vaddr相等*/
    . = 0x10000;

    /*记录只读段的起始地址*/
    PROVIDE(s_text = .);
    /*链接时将所有源文件生成的目标文件的代码段都合并到一个代码段，*为通配符 */
     .text : {   /*冒号前必须有空格 */
        /*格式为： 文件名(段名) ，*为通配符，方便查找到文件 */
        /* *() 为通配符，表示所有文件，*(.text)表示将所有编译好的目标文件的.text段放在一起组成一个大段*/
        *(EXCLUDE_FILE(*first_task* *lib_syscall*) .text)    
        /* EXCLUDE_FILE(文件名) 表示排除该文件*/
    }

    .rodata : {
        *(EXCLUDE_FILE(*first_task* *lib_syscall*) .rodata)
    }
    /* 记录只读段的结束地址 */
    /* 段结束地址为真正结束位置的下一个字节 */
    PROVIDE(e_text = .);


    /* 记录可读写段的起始地址 */
    . = ALIGN(4096); /*让当前地址按4kb对齐*/
    PROVIDE(s_data = .);

    /*直接将.data vaddr表示将.data的虚拟地址设为vaddr, AT(paddr)表示将.data段的物理地址设为paddr*/
    /* .data 0x20000: AT(0){     */
    .data : {    
        *(EXCLUDE_FILE(*first_task* *lib_syscall*) .data) /*将first_task与lib_syscall的各个段排除*/
    }

    .bss : {
        *(EXCLUDE_FILE(*first_task* *lib_syscall*) .bss)
    }

    /*记录紧邻内核四个段的末尾地址的下一个4kb对齐的可用地址*/
    . = ALIGN(4096);
    e_data = .;

    
    /*将第一个进程的各个段放到虚拟地址的0x80000000之后，物理地址紧挨着内核的四个段*/
    /*将进程的各个结放到一个大段 .first_task中*/
    .first_task 0x80000000 : AT(e_data) {
        /*格式为： 文件名(段名) ，*为通配符，方便查找到文件 */
        /*将以下待重定向文件的各个段放在0x80000000开始的地址空间中*/
        *first_task_entry*(.text .rodata .data .bss)
        *first_task*(.text .rodata .data .bss)
        *lib_syscall*(.text .rodata .data .bss)
    }
    /*LOADADDR(段名)：获取该段所在的物理地址paddr*/
    PROVIDE(s_first_task = LOADADDR(.first_task));

    /*SIZEOF(段名): 获取该段的大小size*/
    PROVIDE(e_first_task = LOADADDR(.first_task) + SIZEOF(.first_task)); /*获取在物理地址上紧邻.first_task段的地址*/

   /*记录内核四个段后的紧邻的空间地址, 
    该变量只被链接器认为地址为当前地址 . 
    但并不占用内存，即并不会改变当前地址 . 的值*/
    PROVIDE(mem_free_start = e_first_task);
 
 }

