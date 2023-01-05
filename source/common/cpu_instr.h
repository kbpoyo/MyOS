/**
 * @file cpu_instr.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  对cpu中断指令的封装
 * @version 0.1
 * @date 2023-01-05
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include "types.h"

/**
 * @brief  关闭中断
 * 
 */
static inline void cli (void) {
    __asm__ __volatile__("cli");
}

/**
 * @brief  开启中断
 * 
 */
static inline void sti (void) {
    __asm__ __volatile__("sti");
}


/**
 * @brief  从设备IO端口寄存器(端口号为port)中读取8位数据
 * 
 * @param port 
 * @return uint8_t 
 */
static inline uint8_t inb (uint16_t port) {
    uint8_t rv; //读取的8位数据

    //at&t汇编格式
    __asm__ __volatile__("inb %[p], %[v]"   // in 表示从端口寄存器中读取数据，端口只能用dx表示
                                            // out 表示从端口寄存器中输出数据，端口只能用立即数或dx寄存器表示，
                                            // b表示只读取一个字节(w, l 分别表示2，4字节)
                        :[v]"=a"(rv)        // rv为输出参数用ax 或 al存储， 具体用哪个要看port指定的端口为多少位的寄存器
                        :[p]"d"(port));     // port为输入参数只能用dx寄存器, 
}

#endif