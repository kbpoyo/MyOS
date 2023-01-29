/**
 * @file time.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  初始化定时器
 * @version 0.1
 * @date 2023-01-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef TIME_H
#define TIME_H

#include "common/types.h"

//书p350
//定时器晶体振荡器的频率，即晶体振荡1193182次的时间为1s，PIT:可编程定时器
#define PIT_OSC_FREQ            1193182
//定时器控制信号端口寄存器
#define PIT_COMMAND_MODE_PORT   0x43
//定时器0数据信号端口寄存器
#define PIT_CHANNEL_DATA_PORT  0x40
//选择计数器 0
#define PIT_SELECT_COUNTER      ((uint8_t)(0 << 6))
//选择读取与加载模式
#define PIT_READ_LOAD           ((uint8_t)(3 << 4))
//选择定时器工作模式 3, 使定时器产生周期中断
#define PIT_MODE                ((uint8_t)(3 << 1))

void time_init(void);
//处理定时器中断请求的程序的汇编入口函数声明
void exception_handler_time(void);


#endif