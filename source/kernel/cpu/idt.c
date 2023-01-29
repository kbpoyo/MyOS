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
#include "common/exc_frame.h"
#include "common/cpu_instr.h"
#include "os_cfg.h"
#include "tools/log.h"

//定义中断描述符表
static gate_desc_t idt_table[IDT_TABLE_SIZE];

/**
 * @brief  打印异常栈帧信息
 * 
 * @param frame 栈帧
 */
static void print_exception_fram(const exception_frame_t *frame) {

  log_printf("------------------------stack frame info---------------------");
  log_printf("IRQ:\t\t%d\nerror code:\t%d", frame->num, frame->error_code);
  log_printf("CS:\t\t%d\nDS:\t\t%d\nSS:\t\t%d\nES:\t\t%d\nFS:\t\t%d\nGS:\t\t%d", 
    //TODO:SS暂时没法获取，先用ds替代，之后再进行获取
    frame->cs, frame->ds, frame->ds, frame->es, frame->fs, frame->gs
  );

  log_printf( 
              "EAX:\t\t0x%x\n"
              "EBX:\t\t0x%x\n"
              "ECX:\t\t0x%x\n"
              "EDX:\t\t0x%x\n"
              "ESI:\t\t0x%x\n"
              "EDI:\t\t0x%x\n"
              "EBP:\t\t0x%x\n"
              "ESP:\t\t0x%x", 
              frame->eax, frame->ebx, frame->ecx, frame->edx,
              frame->esi, frame->edi, frame->ebp, frame->esp 
              );

  log_printf("EIP:\t\t0x%x\nEFLAGS:\t\t0x%x", frame->eip, frame->eflags);
}


/**
 * @brief  默认的异常处理函数
 *
 * @param message 异常信息
 * @param fram  异常发生后压入的寄存器信息以及错误代码所组成的栈帧
 */
static void do_default_handler(const exception_frame_t *frame,
                               const char *message) {

  log_printf("---------------------------------------------------");
  log_printf("IRQ/Exception happend: %s", message);
  print_exception_fram(frame);
                              
  
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
int idt_install(const int idt_num, const idt_handler_t handler) {
  // 1.判断IDT下标是否越界
  if (idt_num >= IDT_TABLE_SIZE || idt_num < 0) return -1;

  // 2.在IDT表中设置下标为 idt_num 的中断门
  gate_desc_set(idt_table + idt_num, KERNEL_SELECTOR_CS, (uint32_t)handler,
                GATE_TYPE_INT | GATE_ATTR_P | GATE_ATTR_DPL_0);

  return 0;
}

/**
 * @brief  初始化主从8259芯片
 * 
 */
static void init_pic(void) {
  //1.对主片(8259A芯片)进行初始化, 写入时必须按照ICW1~4的顺序写入
  outb(PIC0_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_IC4);  //ICW1:边缘触发，级联模式，需要ICW4
  outb(PIC0_ICW2, PIC_ICW2_IDT_START);                //ICW2:起始中断向量号为0x20
  outb(PIC0_ICW3, PIC_ICW3_MASTER_CASCADE);           //ICW3:主片用IR2级联从片
  outb(PIC0_ICW4, PIC_ICW4_8086);                     //ICW4:8086模式，正常EOI

  //2.对从片(8259A芯片)进行初始化
  outb(PIC1_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_IC4);
  outb(PIC1_ICW2, PIC_ICW2_IDT_START + 8); //第一块芯片占用了8个中断，所以第二块芯片从第8个中断的下一个开始
  outb(PIC1_ICW3, PIC_ICW3_SLAVE_CASCADE);
  outb(PIC1_ICW4, PIC_ICW4_8086);


  //3.初始化完两块8259芯片后，还需要为每一个中断设置处理程序
  //才可以去接收中断，所以现在要屏蔽中断，IMR位置1则屏蔽该中断请求，0则不屏蔽
  outb(PIC0_IMR, 0xfb); //屏蔽主片除 irq2(第3位) 以外的位传来的中断，(1111 1011)
  outb(PIC1_IMR, 0xff); //屏蔽从片的所有中断


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

/**
 * @brief  开启外部设备的中断
 * 
 * @param irq_num 外部设备对应的中断向量号,即IDT中的下标
 */
void idt_enable(uint8_t irq_num) {
  //1.判断中断请求向量号是否越界
  if (irq_num < PIC_ICW2_IDT_START || irq_num > PIC_ICW2_IDT_START + 14)   return;

  //2.获取到向量号对应的8259A的IRQ标号 主片为0~7 ，从片为 8~15
  irq_num -= PIC_ICW2_IDT_START;

  //3.若在主片上则将主片的IMR寄存器对应位置0，即不屏蔽该中断, 若在从片上也同理
  if (irq_num < 8) {
    uint8_t mask = inb(PIC0_IMR) & ~(1 << irq_num);
    outb(PIC0_IMR, mask);
  } else {
    uint8_t mask = inb(PIC1_IMR) & ~(1 << (irq_num - 8));
    outb(PIC1_IMR, mask);
  }
  
}

/**
 * @brief  关闭外部设备的中断
 * 
 * @param irq_num 外部设备对应的中断向量号，即IDT中的下标
 */
void idt_disable(uint8_t irq_num) {
  //1.判断中断请求向量号是否越界
  if (irq_num < PIC_ICW2_IDT_START || irq_num > PIC_ICW2_IDT_START + 15)   return;

  //2.获取到向量号对应的8259A的IRQ标号 主片为0~7 ，从片为 8~15
  irq_num -= PIC_ICW2_IDT_START;

  //3.若在主片上则将主片的IMR寄存器对应位置1，即屏蔽该中断, 若在从片上也同理
  if (irq_num < 8) {
    uint8_t mask = inb(PIC0_IMR) | (1 << irq_num);
    outb(PIC0_IMR, mask);
  } else {
    uint8_t mask = inb(PIC1_IMR) | (1 << (irq_num - 8));
    outb(PIC1_IMR, mask);
  }
  
}

/**
 * @brief  关闭全局中断
 * 
 */
void idt_disable_global(void) {
  cli();
}

/**
 * @brief  开启全局中断
 * 
 */
void idt_enable_global(void) {
  sti();
}

/**
 * @brief  将OCW2寄存器的7~5位置为 001，用普通的EOI结束方式
 *         向8259A发送EOI命令，8259A会将ISR中优先级最高的位置0
 *
 * @param irq_num 中断向量号，即IDT下标
 */
void pic_send_eoi(int irq_num) {
  //1.获取该中断对应的IRQ标号
  irq_num -= PIC_ICW2_IDT_START;
  //2.判断标号是否越界,若不越界则交给对应芯片处理
  if (irq_num < 0 || irq_num > 15) return;

  //3.若中断来自主片则只需向主片发送EOI
  outb(PIC0_OCW2, PIC_OCW2_EOI);

  //4.若中断来自从片则还需向从片发送EOI
  if (irq_num >= 8) { 
    outb(PIC1_OCW2, PIC_OCW2_EOI);
  }

}