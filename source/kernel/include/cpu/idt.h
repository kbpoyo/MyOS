/**
 * @file idt.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义中断门的数据结构和相关的属性宏，以及中断描述符表
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef IRQ_H
#define IRQ_H

#include "common/types.h"
#include "os_cfg.h"

#pragma pack(1)

//中断门数据结构
typedef struct _gate_desc_t {
    uint16_t offset15_0;    //中断处理程序起始地址对于自己所在段的偏移量的低16位
    uint16_t selector;      //中断处理程序所在段的选择子
    uint16_t attr;          //该中断门的一些属性
    uint16_t offset31_16;   //中断处理程序起始地址对于自己所在段的偏移量的高16位
}gate_desc_t;

#pragma pack()


//定义中断描述符表的大小
#define IDT_TABLE_SIZE 256

//定义中断描述符表
static gate_desc_t idt_table[IDT_TABLE_SIZE];



void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr);

void idt_init(void);

#endif