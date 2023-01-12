/**
 * @file idt.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义中断门的数据结构和相关的属性宏，以及中断描述符表
 * @version 0.1
 * @date 2023-01-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cpu/idt.h"

/**
 * @brief   对没有针对性处理程序的异常进行处理
 *          中断调用要用iret指令返回，所以要进入汇编
 *          这个函数定义在汇编中，在该函数内再调用真正的c处理函数
 *
 */
void exception_handler_unknown(void);

/**
 * @brief  默认的异常处理函数
 * 
 * @param message 异常信息
 * @param fram  异常发生后压入的寄存器信息以及错误代码所组成的栈帧 
 */
static void do_default_handler(const exception_frame_t *fram, const char *message) {
  for (;;) {}
}

/**
 * @brief   对没有针对性处理程序的异常进行处理
 * @param frame 异常发生后压入的寄存器信息以及错误代码所组成的栈帧
 */
void do_handler_unknown(const exception_frame_t *frame) {
  do_default_handler(frame, "unknown exception");

}


/**
 * @brief  设置中断门描述符
 *
 * @param desc 需要被设置的中断门描述符结构
 * @param selector 中断门对应的处理程序所在的段的选择子
 * @param offset 中段门对应的处理程序相对于所在段的偏移量
 * @param attr 中段门的属性
 */
static void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset,
                   uint16_t attr) {
  // 1.初始化偏移量
  desc->offset15_0 = offset & 0xffff;
  desc->offset31_16 = (offset >> 16) & 0xffff;

  // 2.初始化选择子
  desc->selector = selector;

  // 3.初始化属性位
  desc->attr = attr;
}



/**
 * @brief  初始化中断向量表
 * 
 */
void idt_init(void) {
  // 1.初始化IDT中的各个中断门
  for (int i = 0; i < IDT_TABLE_SIZE; ++i) {
    gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS,
                  (uint32_t)exception_handler_unknown,
                  GATE_TYPE_INT | GATE_ATTR_P | GATE_ATTR_DPL_0);
  }

  // 2.加载IDT
  lidt((uint32_t)idt_table, sizeof(idt_table));
}
