/**
 * @file tss.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义任务状态段相关成员与方法
 * @version 0.1
 * @date 2023-01-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TSS_H
#define TSS_H


#include "common/types.h"

#pragma pack(1)

//任务状态段的结构体定义，书p230，p486
typedef struct _tss_t {
    uint32_t pre_link;  //上一个任务的tss段的地址
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;   //特权级提高后，对应特权级使用的栈空间
    uint32_t cr3;   //TODO:cr3寄存器与虚拟页表有关
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;   //各个寄存器
    uint32_t es, cs, ss, ds, fs, gs;    //各个段寄存器
    uint32_t ldt;   //任务对应的局部描述表的位置
    uint32_t iomap; //任务对应的io位图
}tss_t;

#pragma pack()

//定义eflags寄存器使用的宏，供TSS初始化时使用
#define EFLAGS_DEFAULT_1 ((uint32_t)(1 << 1))   //默认为1的位
//IF位，置1开启中断，置0关闭中断
#define EFLAGS_IF ((uint32_t)(1 << 9))          

#endif