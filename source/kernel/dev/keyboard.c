/**
 * @file keyboard.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 键盘设备
 * @version 0.1
 * @date 2023-06-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/keyboard.h"
#include "cpu/idt.h"
#include "common/exc_frame.h"
#include "tools/log.h"
#include "common/cpu_instr.h"

/**
 * @brief 键盘初始化
 * 
 */
void kbd_init(void) {
    idt_install(IRQ1_KEYBOARD, (idt_handler_t)exception_handler_kbd);
    idt_enable(IRQ1_KEYBOARD);
}

/**
 * @brief 键盘中断处理程序
 * 
 */
void do_handler_kbd(exception_frame_t *frame) {
    uint32_t status = inb(KBD_PORT_STAT);
    if (!(status & KBD_STAT_RECV_READY)) {
        pic_send_eoi(IRQ1_KEYBOARD);
        return;

    }

    uint8_t raw_code = inb(KBD_PORT_DATA);
    log_printf("key: %d", raw_code);

    //因为ICW4的EOI位为0，所以要手动发送EOI即中断结束信号
    pic_send_eoi(IRQ1_KEYBOARD);

}