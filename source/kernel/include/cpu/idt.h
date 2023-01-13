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


//定义8259A初始化需要的宏 绑定各个端口寄存器 书p311，p315
//PIC: 可编程控制器，即8259A
//ICW：初始化命令字，每一个8259A里有4个，ICW1~ICW4
//OCW：操作命令字，每一个8259A里有3个，OCW1~OCW3
//初始化8259A，即初始 ICW 和 OCW 这两组寄存器
#define PIC0_ICW1   0x20    //初始化IC4位，ICW1的标志位为 1
#define PIC0_ICW2   0x21    //初始化IRQ接口道中断向量表下标的映射，指定IRQ0对应的下标即可
#define PIC0_ICW3   0x21    //初始化主片哪个端口用作级联
#define PIC0_ICW4   0x21    //初始化处理器类型以及EOI模式(End Of Interrupt)(0：手动结束中断，1：自动结束)

//中断屏蔽端口寄存器，(Interrupt Mask Register), 位宽为8位
#define PIC0_IMR    0x21  

#define PIC1_ICW1   0xa0    
#define PIC1_ICW2   0xa1    
#define PIC1_ICW3   0xa1    
#define PIC1_ICW4   0xa1    
#define PIC1_IMR    0xa1



//p315
//ICW1的第0位为IC4标志位，1：需要初始化ICW4,0：不需要，但x86必须初始化ICW4,即IC4为1
#define PIC_ICW1_IC4        ((uint8_t)(1 << 0))

//ICW1的标志位，置1即可
#define PIC_ICW1_ALWAYS_1   ((uint8_t)(1 << 4))

//外部中断的第一个处理程序在IDT中的下标(由自己确定)，IDT[0x20 ~ 0x2e] 为处理外部中断的中断门,共15个中断
#define PIC_ICW2_IDT_START    0x20

//初始化ICW3, 告诉主片，主片的哪个IRQ接口用作级联，此处用IRQ2(0000 0100), 即第2位 
#define PIC_ICW3_MASTER_CASCADE ((uint8_t)(1 << 2))

//初始化ICW3,告诉从片，主片的哪个IRQ接口用作级联,此处用 IRQ2(0000 0010)，与主片不同，从片用IRQ对应的标号即可
#define PIC_ICW3_SLAVE_CASCADE    0x02

//初始化ICW4的 upm 位， 1：x86处理器， 0：8080或8085处理器
#define PIC_ICW4_8086       ((uint8_t)(1 << 0))

/**
 *  对于EOI模式的理解：
 *    EOI位置1，表示自动结束中断调用，即当cpu第一次发送INTA信号时，8259A将ISR对应位置1，IRR对应位置0
 *    但若此时有优先级更高的中断请求，ISR与IRR都将撤销之前的操作以响应更高优先级的中断， 但若在cpu第二次
 *    发送的INTA信号，即等待中断向量号，被响应之后，有更高优先级的中断请求进入则可能扰乱正在服务的程序
 * 
 *    EOI位置0，表示在当前中断程序结束时手动写入操作命令字OCW2,向8259A发送中断结束命令，8259A将自动清除当前
 *    ISR中最高优先级的位，而保存在ISR中的最高优先级的位一定就是对应此时正在运行的中断程序，所以不会有问题
 * 
 */




//因为中断调用需要使用 iret 指令进行返回，所以要进入汇编后再调用实际进行处理的c函数
//以下为异常处理的汇编入口函数声明
void exception_handler_unknown (void);              //unknown
void exception_handler_divider (void);              //IDT0
void exception_handler_debug (void);                //IDT1
void exception_handler_nmi (void);                  //IDT2
void exception_handler_breakpoint (void);           //IDT3
void exception_handler_overflow (void);             //IDT4
void exception_handler_bound_range (void);          //IDT5
void exception_handler_invalid_opcode (void);       //IDT6
void exception_handler_device_unavailable (void);   //IDT7
void exception_handler_double_fault (void);         //IDT8
void exception_handler_invalid_tss (void);          //IDT10
void exception_handler_segment_not_present (void);  //IDT11
void exception_handler_stack_segment_fault (void);  //IDT12
void exception_handler_general_protection (void);   //IDT13
void exception_handler_page_fault (void);           //IDT14
void exception_handler_fpu_error (void);            //IDT16
void exception_handler_alignment_check (void);      //IDT17
void exception_handler_machine_check (void);        //IDT18
void exception_handler_smd_exception (void);        //IDT19
void exception_handler_virtual_exception (void);    //IDT20
void exception_handler_control_exception (void);    //IDT21


#endif