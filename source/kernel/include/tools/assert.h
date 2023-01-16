/**
 * @file assert.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义ASSERT宏，方便在debug版本下进行调试
 * @version 0.1
 * @date 2023-01-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ASSERT_H
#define ASSERT_H


//定义ASSERT宏，方便在debug版本下进行调试
#ifndef RELEASE
#define ASSERT(expr) \
    if (!(expr)) pannic(__FILE__, __LINE__, __func__, #expr)
void pannic(const char *file, int line, const char *func, const char *reason);

#else
#define ASSERT(expr) ((void)0)
#endif

#endif 