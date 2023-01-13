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


//==============================真正进行异常处理的c程序==================================
/**
 * @brief  默认的异常处理函数
 * 
 * @param message 异常信息
 * @param fram  异常发生后压入的寄存器信息以及错误代码所组成的栈帧 
 */
static void do_default_handler(const exception_frame_t *frame, const char *message) {
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
 * @brief  除0异常
 * 
 * @param frame 异常栈帧
 */
void do_handler_divider (exception_frame_t *frame) {
  do_default_handler(frame, "divider exception");
}

//==============================真正进行异常处理的c程序==================================










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
 * @brief  将异常的下标与异常处理程序绑定
 * 
 * @param idt_num 异常的下标
 * @param handler 异常处理程序的偏移地址
 * @return int 成功返回 0 失败放回 -1
 */
static int idt_install(int idt_num, idt_handler_t handler) {
  //1.判断IDT下标是否越界
  if (idt_num >= IDT_TABLE_SIZE) return -1;

  //2.在IDT表中设置下标为 idt_num 的中断门
  gate_desc_set(idt_table + idt_num, KERNEL_SELECTOR_CS, (uint32_t)handler,
                GATE_TYPE_INT | GATE_ATTR_P | GATE_ATTR_DPL_0);

  return 0;

}



/**
 * @brief  初始化中断向量表
 * 
 */
void idt_init(void) {
  // 1.初始化IDT中的各个中断门(未知异常类型)
  for (int i = 0; i < IDT_TABLE_SIZE; ++i) {
   idt_install(i, (idt_handler_t)exception_handler_unknown);
  }

  //2.绑定异常中断向量表中对应下标的中断门的处理函数
  idt_install(IDT0_DE, (idt_handler_t)exception_handler_divider);

  //3.加载IDT
  lidt((uint32_t)idt_table, sizeof(idt_table));
}
