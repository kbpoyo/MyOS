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
 * @brief  默认的异常处理函数
 *
 * @param message 异常信息
 * @param fram  异常发生后压入的寄存器信息以及错误代码所组成的栈帧
 */
static void do_default_handler(const exception_frame_t *frame,
                               const char *message) {
  for (;;) {
    hlt();
  }
}

//==============================真正进行异常处理的c程序==================================

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
void do_handler_divider(const exception_frame_t *frame) {
  do_default_handler(frame, "divider exception");
}

void do_handler_debug(const exception_frame_t *frame) {
  do_default_handler(frame, "debug exception");
}

void do_handler_nmi(const exception_frame_t *frame) {
  do_default_handler(frame, "nmi exception");
}
void do_handler_breakpoint(const exception_frame_t *frame) {
  do_default_handler(frame, "breakpoint exception");
}
void do_handler_overflow(const exception_frame_t *frame) {
  do_default_handler(frame, "overflow exception");
}
void do_handler_bound_range(const exception_frame_t *frame) {
  do_default_handler(frame, "bound_range exception");
}
void do_handler_invalid_opcode(const exception_frame_t *frame) {
  do_default_handler(frame, "invalid_opcode exception");
}
void do_handler_device_unavailable(const exception_frame_t *frame) {
  do_default_handler(frame, "device_unavailable exception");
}
void do_handler_double_fault(const exception_frame_t *frame) {
  do_default_handler(frame, "double_fault exception");
}
void do_handler_invalid_tss(const exception_frame_t *frame) {
  do_default_handler(frame, "invalid_tss exception");
}
void do_handler_segment_not_present(const exception_frame_t *frame) {
  do_default_handler(frame, "segment_not_present exception");
}
void do_handler_stack_segment_fault(const exception_frame_t *frame) {
  do_default_handler(frame, "stack_segment_fault exception");
}
void do_handler_general_protection(const exception_frame_t *frame) {
  do_default_handler(frame, "general_protection exception");
}
void do_handler_page_fault(const exception_frame_t *frame) {
  do_default_handler(frame, "page_fault exception");
}
void do_handler_fpu_error(const exception_frame_t *frame) {
  do_default_handler(frame, "fpu_error exception");
}
void do_handler_alignment_check(const exception_frame_t *frame) {
  do_default_handler(frame, "alignment_check exception");
}
void do_handler_machine_check(const exception_frame_t *frame) {
  do_default_handler(frame, "machine_check exception");
}
void do_handler_smd_exception(const exception_frame_t *frame) {
  do_default_handler(frame, "smd_exception exception");
}
void do_handler_virtual_exception(const exception_frame_t *frame) {
  do_default_handler(frame, "virtual_exception exception");
}
void do_handler_control_exception(const exception_frame_t *frame) {
  do_default_handler(frame, "control_exception exception");
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
static void gate_desc_set(gate_desc_t *desc, const uint16_t selector, const uint32_t offset,
                          const uint16_t attr) {
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
static int idt_install(const int idt_num, const idt_handler_t handler) {
  // 1.判断IDT下标是否越界
  if (idt_num >= IDT_TABLE_SIZE) return -1;

  // 2.在IDT表中设置下标为 idt_num 的中断门
  gate_desc_set(idt_table + idt_num, KERNEL_SELECTOR_CS, (uint32_t)handler,
                GATE_TYPE_INT | GATE_ATTR_P | GATE_ATTR_DPL_0);

  return 0;
}

static void init_pic(void) {
  //1.对第一块8259芯片进行初始化
  outb(PIC0_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
  outb(PIC0_ICW2, IDT_PIC_START);
  outb(PIC0_ICW3, 1 << 2);
  outb(PIC0_ICW4, PIC_ICW4_8086);

  //2.对第二块8259芯片进行初始化
  outb(PIC1_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
  outb(PIC1_ICW2, IDT_PIC_START + 8); //第一块芯片占用了8个中断，所以第二块芯片从第8个中断的下一个开始
  outb(PIC1_ICW3, 2);
  outb(PIC1_ICW4, PIC_ICW4_8086);
  

  //3.初始化完两块8259芯片后，还需要为每一个中断设置处理程序
  //才可以去接收中断，所以现在要屏蔽中断
  outb(PIC0_IMR, 0xff & ~(1 << 2)); //屏蔽第一块芯片除 irq2 位以为的位传来的中断，irq2位用来连接两块芯片
  outb(PIC0_IMR, 0xff); //屏蔽第二块芯片的所有中断
  
  

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

  // 2.绑定异常中断向量表中对应下标的中断门的处理函数
  idt_install(IDT0_DE, (idt_handler_t)exception_handler_divider);
  idt_install(IDT1_DB, (idt_handler_t)exception_handler_debug);
  idt_install(IDT2_NMI, (idt_handler_t)exception_handler_nmi);
  idt_install(IDT3_BP, (idt_handler_t)exception_handler_breakpoint);
  idt_install(IDT4_OF, (idt_handler_t)exception_handler_overflow);
  idt_install(IDT5_BR, (idt_handler_t)exception_handler_bound_range);
  idt_install(IDT6_UD, (idt_handler_t)exception_handler_invalid_opcode);
  idt_install(IDT7_NM, (idt_handler_t)exception_handler_device_unavailable);
  idt_install(IDT8_DF, (idt_handler_t)exception_handler_double_fault);
  idt_install(IDT10_TS, (idt_handler_t)exception_handler_invalid_tss);
  idt_install(IDT11_NP, (idt_handler_t)exception_handler_segment_not_present);
  idt_install(IDT12_SS, (idt_handler_t)exception_handler_stack_segment_fault);
  idt_install(IDT13_GP, (idt_handler_t)exception_handler_general_protection);
  idt_install(IDT14_PF, (idt_handler_t)exception_handler_page_fault);
  idt_install(IDT16_MF, (idt_handler_t)exception_handler_fpu_error);
  idt_install(IDT17_AC, (idt_handler_t)exception_handler_alignment_check);
  idt_install(IDT18_MC, (idt_handler_t)exception_handler_machine_check);
  idt_install(IDT19_XM, (idt_handler_t)exception_handler_smd_exception);
  idt_install(IDT20_VE, (idt_handler_t)exception_handler_virtual_exception);
  idt_install(IDT21_CP, (idt_handler_t)exception_handler_control_exception);

  //3.加载IDT
  lidt((uint32_t)idt_table, sizeof(idt_table));

  //4.初始化8259设备中断芯片
  init_pic();
}
