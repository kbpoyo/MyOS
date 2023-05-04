/**
 * @file gdt.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义段描述的数据结构以及相关属性位的宏，以及全局描述符表
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

}segment_desc_t, tss_desc_t;


#pragma pack()

/**
 * @brief   定义全局描述符表的大小，理论最大为8192
 *          全局描述符表用 lgdt （48位内存数据）来加载
 *          前16位为GDT的大小，即65536字节，后32为其起始地址
 *          由于每一个段描述符大小为8字节，所以GDT所含段描述符最多为 65536/8 = 8192
 * 
 */
#define GDT_TABLE_SIZE 8192


//由高位到低位定义段标识符的属性标志位
//15位：G标志位, 0:段界限粒度为 1 字节  1:粒度为 4kb 的 
#define SEG_ATTR_G ((uint16_t)(1 << 15))
//14位：若为代码段则为D标志位，若为栈段则为B标志位，0:cpu以16位模式处理该段，1:cpu以32位模式处理该段
#define SEG_ATTR_D_OR_B ((uint16_t)(1 << 14))

//13位：L标志位, 1:该段为64位，0:该段为32位，保留置0即可
//12位：AVL 标志位： 没有专门用途
//11~8位：中间4位为段界限的高4位

//7位：P 标志位：0:该段不在内存中，1:该段在内存中
//cpu负责检查该位，若为0则抛出异常，需要进行异常处理后将其置1
//用来将不常用的段在内存与磁盘中进行置换
#define SEG_ATTR_P ((uint16_t)(1 << 7))
//6~5位：DPL位，标志段的特权级(0,1,2,3)
#define SEG_ATTR_DPL_0 ((uint16_t)(0 << 6))   //最高特权级
#define SEG_ATTR_DPL_3 ((uint16_t)(3 << 5))   //最低特权级
//4位：S位，0是系统段, 1是非系统段，书p153
#define SEG_ATTR_S_SYSTEM ((uint16_t)(0 << 4))//系统段
#define SEG_ATTR_S_NORMAL ((uint16_t)(1 << 4))//非系统段
//3~0位：type位，标志文件类型，与S位一起决定文件特性, 第0位忽略
#define SEG_ATTR_TYPE_CODE ((uint16_t)(1 << 3))//当为非系统段时，标志该段为代码段
#define SEG_ATTR_TYPE_DATA ((uint16_t)(0 << 3))//当为非系统段时，标志该段为数据段
#define SEG_ATTR_TYPE_RW ((uint16_t)(1 << 1))  //若是代码段则可读，若是数据段则可读写
//标志该段为TSS段, 10B1, B为1表示该任务忙碌，不可启用，为0表示该任务空闲，可启用  
#define SEG_ATTR_TYPE_TSS ((uint16_t)(9 << 0)) 

//定义选择子的低2位CPL权限位
#define SEG_CPL0    (0 << 0)
#define SEG_CPL3    (3 << 0)

void segment_desc_set(uint16_t selector,  uint32_t base, uint32_t limit, uint16_t attr);
void gdt_init(void);
int gdt_alloc_desc();

#endif