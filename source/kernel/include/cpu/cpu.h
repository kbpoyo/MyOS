/**
 * @file cpu.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  放置x86系统需要的一些数据结构
 * @version 0.1
 * @date 2023-01-09
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CPU_H
#define CPU_H

#include "common/types.h"

//指定编译器按一字节对齐，防止结构体中元素在内存空间中不连续
#pragma pack(1)

//段描述符的数据结构， 书p151
typedef struct _segment_desc_t {
    uint16_t limit15_0; //段界限的低16位
    uint16_t base15_0;  //段基址的低16位
    uint8_t base23_16;  //段基址的中8位
    uint16_t attr;      //中间0~7和12~15为属性位，8~11为段界限的高4位
    uint8_t base31_24;  //段基址的高8位

}segment_desc_t;

#pragma pack()

void segment_desc_set(uint16_t selector,  uint32_t base, uint32_t limit, uint16_t attr);

void cpu_init(void);


#endif