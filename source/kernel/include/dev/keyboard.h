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

#include "common/types.h"




//定义键盘映射结构
typedef struct _key_map_t {
    uint8_t normal; //当单个键按下时，该键获取到的值
    uint8_t func;//当有功能键配合按下时，获取到的值

}key_map_t;

//定义功能键的原始键值码
#define KEY_RSHIFT  0x36
#define KEY_LSHIFT  0x2a
#define KEY_CAPS    0x3a

#define KEY_CTRL    0x1d
#define KEY_ALT     0x38

#define KEY_F1      0x3b
#define KEY_F2      0x3c
#define KEY_F3      0x3d
#define KEY_F4      0x3e
#define KEY_F5      0x3f
#define KEY_F6      0x40
#define KEY_F7      0x41
#define KEY_F8      0x42
#define KEY_F9      0x43
#define KEY_F10     0x44
#define KEY_F11     0x57
#define KEY_F12     0x58


#define KEY_E0  0xe0
#define KEY_E1  0xe1

//定义键盘当前按键状态
typedef struct _kbd_state_t {
    int caps_lock : 1;
    int lshift_press : 1;
    int rshift_press : 1;
    int lalt_press : 1;
    int ralt_press : 1;
    int lctrl_press : 1;
    int rctrl_press : 1;


}kbd_state_t;



#define KBD_PORT_DATA   0x60    //数据读取端口
#define KBD_PORT_STAT   0x64    //状态读取端口
#define KBD_PORT_CMD    0x64    //命令写入端口
//定义键盘8位状态值的宏
#define KBD_STAT_RECV_READY (1 << 0)    //0位：0=无数据，1=有数据


void kbd_init(void);
void exception_handler_kbd(void);

#endif