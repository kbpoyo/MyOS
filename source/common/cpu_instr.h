/**
 * @file cpu_instr.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  对cpu中断指令的封装, 使用的汇编格式为at&t汇编格式(gcc编译器的格式)
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
static inline void cli(void) { __asm__ __volatile__("cli"); }

/**
 * @brief  开启中断
 *
 */
static inline void sti(void) { __asm__ __volatile__("sti"); }

/**
 * @brief  暂停cpu的运行,ip寄存器指向下一条指令
 *         此时cpu可以接收中断
 *
 */
static inline void hlt(void) { __asm__ __volatile__("hlt"); }

/**
 * @brief  从设备IO端口寄存器(端口号为port)中读取8位数据
 *
 * @param port
 * @return uint8_t
 */
static inline uint8_t inb(uint16_t port) {
  uint8_t rv;  // 读取的8位数据

  __asm__ __volatile__(
      "inb %[p], %[v]"  // in 表示从端口寄存器中读取数据，端口只能用dx存放
                        // out
                        // 表示往端口寄存器中写入数据，端口只能用立即数或dx寄存器表示，
                        // b表示只读取一个字节(w, l 分别表示2，4字节)
      : [v] "=a"(rv)  // rv为输出参数用ax 或 al存储，
                      // 具体用哪个要看port指定的端口为多少位的寄存器
      : [p] "d"(port));  // port为输入参数只能用dx寄存器,

  return rv;
}

/**
 * @brief  往IO端口寄存器port中写入八位数据data
 *
 * @param port
 * @param data
 */
static inline void outb(uint16_t port, uint8_t data) {
  __asm__ __volatile__(
      "outb %[v], %[p]"
      :  // 无输出参数
      : [p] "d"(port),
        [v] "a"(data));  // out 指令只能使用dx寄存器或立即数指定端口，
                         // 只能用ax 或
                         // al来作为数据输入，具体用哪个需要看端口寄存器是几位
}

/**
 * @brief  往IO端口寄存器port中写入16位数据data
 *
 * @param port
 * @param data
 */
//TODO:note-> outw只写入1字节数据，导致磁盘未接收到一个扇区的数据
//从而一直未触发中断
static inline void outw(uint16_t port, uint16_t data) {
  __asm__ __volatile__(
      "out %[v], %[p]"
      :  // 无输出参数
      : [p] "d"(port),
        [v] "a"(data));  // out 指令只能使用dx寄存器或立即数指定端口，
                         // 只能用ax 或
                         // al来作为数据输入，具体用哪个需要看端口寄存器是几位
}

/**
 * @brief  从端口(port)寄存器中读取16位数据
 *
 * @param port
 * @return uint16_t
 */
static inline uint16_t inw(uint16_t port) {
  uint16_t rv;  // 读取的16位数据

  __asm__ __volatile__("in %[p], %[v]"  // in默认就是inw, 读取一个字即16位
                       : [v] "=a"(rv)
                       : [p] "d"(port));

  return rv;
}

/**
 * @brief  加载全局描述符表
 *
 * @param start 加载到内存中的位置
 * @param size 所占内存空间
 */
static inline void lgdt(uint32_t start, uint32_t size) {
  struct {
    uint16_t limit;       // GDT的在内存中的逻辑边界
    uint16_t start15_0;   // GDT地址的低16位
    uint16_t start31_16;  // GDT地址的高16位
  } gdt;

  gdt.start31_16 = (start >> 16) & 0xffff;  // 初始化GDT地址的高16位
  gdt.start15_0 = start & 0xffff;           // 初始化GDT地址的低16位
  gdt.limit =
      size -
      1;  // 初始化GDT的在内存中的逻辑边界，根据首地址进行偏移所以是size - 1

  __asm__ __volatile__(
      "lgdt %[g]"
      :
      : [g] "m"(gdt));  //"m"表示指令直接作用于该内存位置
                        // 即此处表示lgdt直接加载gdt所在的内存区域的内容
}

/**
 * @brief  加载中断描述符表
 *
 * @param start 加载到内存中的位置
 * @param size 所占内存空间
 */
static inline void lidt(uint32_t start, uint32_t size) {
  struct {
    uint16_t limit;       // IDT的在内存中的逻辑边界
    uint16_t start15_0;   // IDT地址的低16位
    uint16_t start31_16;  // IDT地址的高16位
  } idt;

  idt.start31_16 = (start >> 16) & 0xffff;  // 初始化IDT地址的高16位
  idt.start15_0 = start & 0xffff;           // 初始化IDT地址的低16位
  idt.limit =
      size -
      1;  // 初始化IDT的在内存中的逻辑边界，根据首地址进行偏移所以是size - 1

  __asm__ __volatile__(
      "lidt %[g]"
      :
      : [g] "m"(idt));  //"m"表示指令直接作用于该内存位置
                        // 即此处表示lidt直接加载idt所在的内存区域的内容
}

/**
 * @brief  读取cr0寄存器的值
 *
 * @return uint32_t
 */
static inline uint32_t read_cr0(void) {
  uint32_t cr0;
  __asm__ __volatile__("mov %%cr0, %[v]" : [v] "=r"(cr0));
  return cr0;
}

/**
 * @brief  向cr0寄存器写入数据
 *CR0 是系统内的控制寄存器之一。控制寄存器是一些特殊的寄存器，它们可以控制 CPU 的一些重要特性。
 *CR0 中包含了 6 个预定义标志：
 *第 0 位是保护允许位 PE (Protected Enable)，用于启动保护模式，如果 PE 位置 1，则保护模式启动，如果 PE=0，则在实模式下运行。
 *第 31 位是分页允许位 (Paging Enable)，它表示芯片上的分页部件是否允许工作。
 * @param data
 */
static inline void write_cr0(uint32_t data) {
  __asm__ __volatile__("mov %[v], %%cr0" : : [v] "r"(data));
}

/**
 * @brief  读取cr2寄存器的值，cr2寄存器在发生page_fault异常时会记录触发异常的访问地址
 *CR2 寄存器用于发生页异常时报告出错信息。当发生页异常时，处理器把引起页异常的线性地址保存在 CR2 中。
 *操作系统中的页异常处理程序可以检查 CR2 的内容，从而查出线性地址空间中的哪一页引起本次异常²。
 * @return uint32_t
 */
static inline uint32_t read_cr2(void) {
  uint32_t cr2;
  __asm__ __volatile__("mov %%cr2, %[v]" : [v] "=r"(cr2));
  return cr2;
}



/**
 * @brief  读取cr3寄存器的值
 *
 * @return uint16_t
 */
static inline uint32_t read_cr3(void) {
  uint32_t cr3;
  __asm__ __volatile__("mov %%cr3, %[v]" : [v] "=r"(cr3));
  return cr3;
}

/**
 * @brief  向cr3寄存器写入数据
 * CR3 寄存器用于控制和确定处理器的操作模式以及当前执行任务的特性。
 * 它含有存放页目录表页面的物理地址，
 * 因此 CR3 也被称为 PDBR（Page-Directory Base address Register）。
 *
 * @param data
 */
static inline void write_cr3(uint32_t data) {
  __asm__ __volatile__("mov %[v], %%cr3" : : [v] "r"(data));
}

/**
 * @brief  读取cr4寄存器的值
 *
 * @return uint16_t
 */
static inline uint32_t read_cr4(void) {
  uint32_t cr4;
  __asm__ __volatile__("mov %%cr4, %[v]" : [v] "=r"(cr4));
  return cr4;
}

/**
 * @brief  向cr4寄存器写入数据
 *
 * @param data
 */
static inline void write_cr4(uint32_t data) {
  __asm__ __volatile__("mov %[v], %%cr4" : : [v] "r"(data));
}

/**
 * @brief  远跳转，当跳转发生在TSS段之间时，cpu将会保存状态到当前TR寄存器指向的TSS段
 *
 * @param selector 选择子
 * @param offset 跳转的偏移量
 */
static inline void far_jump(uint32_t selector, uint32_t offset) {
  // 传入远跳转需要的参数, 即cs = selector, eip = offset 
  uint32_t addr[] = {offset, selector};  

  __asm__ __volatile__("ljmpl *(%[a])" : : [a] "r"(addr));
}

/**
 * @brief   写入当前任务的TSS段描述符的选择子到TR寄存器中，告诉cpu当前段的TSS位置
 *          以便在进行任务切换时，将状态保存到该TSS段中
 * 
 * @param tss_selector 准备运行的任务的TSS段的选择子
 */
static inline void write_tr(uint16_t tss_selector) {
  __asm__ __volatile__("ltr %[v]" : : [v]"r"(tss_selector));
}

/**
 * @brief  读取当前cpu的eflags寄存器
 * 
 * @return uint32_t 放回eflags的值
 */
static inline uint32_t read_eflags(void) {
  uint32_t eflags;
  //pushf 压入eflags的值到栈中
  __asm__ __volatile__("pushf\n\tpop %%eax":"=a"(eflags));
  
  return eflags;
}

/**
 * @brief  将状态state写入eflags寄存器
 * 
 * @param state 
 */
static inline void write_eflags(uint32_t state) {
  __asm__ __volatile__("push %%eax\n\tpopf"::"a"(state));
}

#endif