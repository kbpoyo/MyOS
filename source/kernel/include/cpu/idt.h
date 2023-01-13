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
#include "common/exc_frame.h"
#include "common/cpu_instr.h"
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


//定义中断异常处理的函数指针
typedef void (*idt_handler_t)(exception_frame_t *frame);


void idt_init(void);

//定义中断类型与中断向量表的下标绑定, 这是在 Intel 手册里面指定的
#define IDT0_DE     0   //除0异常
#define IDT1_DB     1
#define IDT2_NMI    2
#define IDT3_BP     3
#define IDT4_OF     4
#define IDT5_BR     5
#define IDT6_UD     6
#define IDT7_NM     7
#define IDT8_DF     8
#define IDT10_TS    10
#define IDT11_NP    11
#define IDT12_SS    12
#define IDT13_GP    13
#define IDT14_PF    14
#define IDT16_MF    16
#define IDT17_AC    17
#define IDT18_MC    18
#define IDT19_XM    19
#define IDT20_VE    20
#define IDT21_CP    21

//因为中断调用需要使用 iret 指令进行返回，所以要进入汇编后再调用实际进行处理的c函数
//以下为异常处理的汇编入口函数声明
void exception_handler_unknown (void);
void exception_handler_divider (void);
void exception_handler_Debug (void);
void exception_handler_NMI (void);
void exception_handler_breakpoint (void);
void exception_handler_overflow (void);
void exception_handler_bound_range (void);
void exception_handler_invalid_opcode (void);
void exception_handler_device_unavailable (void);
void exception_handler_double_fault (void);
void exception_handler_invalid_tss (void);
void exception_handler_segment_not_present (void);
void exception_handler_stack_segment_fault (void);
void exception_handler_general_protection (void);
void exception_handler_page_fault (void);
void exception_handler_fpu_error (void);
void exception_handler_alignment_check (void);
void exception_handler_machine_check (void);
void exception_handler_smd_exception (void);
void exception_handler_virtual_exception (void);
void exception_handler_control_exception (void);


#endif