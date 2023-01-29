/**
 * @file time.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  初始化定时器
 * @version 0.1
 * @date 2023-01-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/time.h"
#include "common/cpu_instr.h"
#include "common/exc_frame.h"
#include "os_cfg.h"
#include "cpu/idt.h"

static uint32_t sys_tick = 0;

/**
 * @brief  处理定时器中断的c函数
 * 
 * @param frame 异常栈帧
 */
void do_handler_time(const exception_frame_t *frame) {
    sys_tick++;

    //因为ICW4的EOI位为0，所以要手动发送EOI即中断结束信号
    pic_send_eoi(IRQ0_TIMER);

    //TODO:运行完一个时间片，判断是否需要执行任务切换，若需要则执行
    task_time_tick();
}


/**
 * @brief  初始化可编程定时器
 * 
 */
static void init_pit(void) {
    //1.选择定时器的计算器，读取与加载初始值模式，以及工作模式
    outb(PIT_COMMAND_MODE_PORT, PIT_SELECT_COUNTER | PIT_READ_LOAD | PIT_MODE);

    //2.写入16位初始值,即一个中断后期晶体的振荡次数，由确定的加载方式知，先写入低8位，再写入高8位
    uint16_t reload_count = (PIT_OSC_FREQ / 1000) * OS_TICKS_MS;   //确定晶体一个中断周期的震荡次数
    outb(PIT_CHANNEL_DATA_PORT, reload_count & 0xff);
    outb(PIT_CHANNEL_DATA_PORT, (reload_count >> 8) & 0xff);

    //3.绑定定时器中断的异常处理程序,并开启该中断
    idt_install(IRQ0_TIMER, (idt_handler_t)exception_handler_time);
    idt_enable(IRQ0_TIMER);

}


/**
 * @brief  初始化定时器
 * 
 */
void time_init(void) {
    sys_tick = 0;
    init_pit();
}