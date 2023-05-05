/**
 * @file exc_frame.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  异常栈帧信息
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef EXC_FRAME_H
#define EXC_FRAME_H

#include "types.h"

//存储异常发生后压入的寄存器信息以及错误代码所组成的栈帧
//根据压入的顺序定义结构体
typedef struct _exception_frame_t {
    
    uint32_t gs, fs, es, ds;    //手动压入的段寄存器
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; //pusha指令压入的通用寄存器
    uint32_t num;//手动压入中断向量表的下标
    //异常中断发生后，cpu自动压入的寄存器以及错误代码
    //若原始特权级为3，cpu还会进行特权级切换,即将cs寄存器切换为之前运行的内核cs段选择子，ss3切换为ss0选择子, eps3切换到esp0
    uint32_t error_code, eip, cs, eflags;   
    uint32_t esp3, ss3;     //由用户程序触发异常中断时，
                            //cpu进行特权级切换，由ss3，esp3到ss0,esp0,
                            //cpu会自动将原始的esp3和ss3压入栈中 
    
}exception_frame_t;


#endif

  