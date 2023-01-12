/**
 * @file os_cfg.h
 * @author kbpoyo (kbpoyo.com)
 * @brief 定义与操作系统有关的数据
 * @version 0.1
 * @date 2023-01-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef OS_CFG_H
#define OS_CFG_H
/**
 * @brief   定义全局描述符表的大小，理论最大为8192
 *          全局描述符表用 lgdt （48位内存数据）来加载
 *          前16位为GDT的大小，即65536字节，后32为其起始地址
 *          由于每一个段描述符大小为8字节，所以GDT所含段描述符最多为 65536/8 = 8192
 * 
 */
#define GDT_TABLE_SIZE 8192

//内核代码段的选择子,gdt_table[1]
#define KERNEL_SELECTOR_CS (0x0001 << 3)

//内核数据段的选择子,gdt_table[2]
#define KERNEL_SELECTOR_DS (0x0002 << 3)

//内核栈空间的大小为8kb
#define KERNEL_STACK_SIZE (8 * 1024)

#endif