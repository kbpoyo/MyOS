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


//定义中断门的属性位
//15位：P标志为，该中断门及其处理程序是否存在于内存
#define GATE_ATTR_P ((uint16_t)(1 << 15))
//14~13位：DPL标志位，标志该中断门的特权级
#define GATE_ATTR_DPL_0 ((uint16_t)(0 << 14))   //最高特权级
#define GATE_ATTR_DPL_3 ((uint16_t)(3 << 13))   //最低特权级
//11位：D标志位，0表示16位模式，1表示32位模式，当前为保护模式直接置1即可
//10~8位：取110标志该门为中断门(Interrupt Gate),不用关心Task Gate 和 Trap Gate
#define GATE_TYPE_INT ((uint16_t)(0xe << 8))    //因为11~8位已经确定所以将11~8位一起初始化



void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr);

void idt_init(void);

#endif