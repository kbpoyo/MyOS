/**
 * @file syscall.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 定义系统调用相关参数和数据结构
 * @version 0.1
 * @date 2023-05-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef SYSCALL_H
#define SYSCALL_H

//定义调用门描述符中，参数个数属性，即系统调用函数的参数个数
#define SYSCALL_PARAM_COUNT 5

void exception_handler_syscall (void);

#endif