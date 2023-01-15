/**
 * @file log.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  打印日志相关函数
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LOG_H
#define LOG_H

#include "common/types.h"
#include "common/cpu_instr.h"

//定义串行端口(串口)，用于信息输出
#define COM1_PORT 0x3f8



void log_init(void);
void log_printf(const char *formate, ...);


#endif