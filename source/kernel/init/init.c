/**
 * @file init.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  对内核进行初始化
 * @version 0.1
 * @date 2023-01-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "init.h"
#include "common/boot_info.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "dev/time.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/assert.h"

/**
 * @brief  对内核进行初始化操作
 * 
 */
void kernel_init(boot_info_t* boot_info) {
    //1.初始化并重新加载GDT表
    gdt_init();

    //2.初始化日志程序,便于后期调用
    log_init();

    //3.初始化并加载中断描述符表IDT
    idt_init();

    //4.初始化定时器的中断处理
    time_init();


    ASSERT(boot_info->ram_region_count == 0);


    //初始化完成后将在汇编里重新加载内核代码段与数据段的选择子，并为内核程序分配栈空间

}


void init_main(void) {

    log_printf("Kernel is running......");
    log_printf("Name: %s, Version: %s", "kbpoyoOS", OS_VERSION);
    log_printf("dex 10 = %d, hex 10 = %x bin: 10 = %b char: %c", 65536, 0, 65536, 'd');
    //idt_enable_global(); 

    int a = 10 / 0;


    for (;;){}
}