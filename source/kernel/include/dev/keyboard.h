/**
 * @file keyboard.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 键盘设备
 * @version 0.1
 * @date 2023-06-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_PORT_DATA   0x60    //数据读取端口
#define KBD_PORT_STAT   0x64    //状态读取端口
#define KBD_PORT_CMD    0x64    //命令写入端口

//定义键盘8位状态值的宏
#define KBD_STAT_RECV_READY (1 << 0)    //0位：0=无数据，1=有数据


void kbd_init(void);
void exception_handler_kbd(void);

#endif