/**
 * @file gate.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 定义门结构及相关宏和方法
 * @version 0.1
 * @date 2023-05-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef GATE_H
#define GATE_H

#include "common/types.h"


#pragma pack(1)

//门数据结构
typedef struct _gate_desc_t {
    uint16_t offset15_0;    //门处理程序起始地址对于自己所在段的偏移量的低16位
    uint16_t selector;      //门处理程序所在段的选择子
    uint16_t attr;          //该门描述符的一些属性
    uint16_t offset31_16;   //门处理程序起始地址对于自己所在段的偏移量的高16位
}gate_desc_t;

#pragma pack()




//定义门的属性位
//15位：P标志为，该中断门及其处理程序是否存在于内存
#define GATE_ATTR_P ((uint16_t)(1 << 15))
//14~13位：DPL标志位，标志该中断门的特权级
#define GATE_ATTR_DPL_0 ((uint16_t)(0 << 14))   //最高特权级
#define GATE_ATTR_DPL_3 ((uint16_t)(3 << 13))   //最低特权级
//11位：D标志位，0表示16位模式，1表示32位模式，当前为保护模式直接置1即可
//10~8位：不用关心Task Gate 和 Trap Gate, 只用关心中断门和调用门
//因为11~8位已经确定所以将11~8位一起初始化
#define GATE_TYPE_INT ((uint16_t)(0xe << 8))    //取110标志该门为中断门(Interrupt Gate)
#define GATE_TYPE_SYSCALL   ((uint16_t)(0xc << 8))  //取100标志该门为调用门(Call Gate)

void gate_desc_set(gate_desc_t *desc, const uint16_t selector, const uint32_t offset,
                          const uint16_t attr);

#endif