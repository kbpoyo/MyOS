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
    uint32_t num, error_code;   //手动压入的错误码和中断向量表的下标
    uint32_t eip, cs, eflags;  //异常中断发生后，cpu自动压入的寄存器以及错误代码
    
}exception_frame_t;


#endif

  