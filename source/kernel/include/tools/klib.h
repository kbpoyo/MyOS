/**
 * @file klib.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  自键一些便捷的c库函数
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef KLIB_H
#define KLIB_H

#include <stdarg.h>
#include "common/types.h"

void kernel_strcpy(char *dest, const char *src);
void kernel_strncpy(char* dest, const char *src, int size);
int  kernel_strncmp(const char *s1, const char *s2, int size);
void kernel_itoa(char *buf, int num, int base);

void kernel_memcpy(void *dest, const void *src, int size);
void kernel_memset(void *dest, uint8_t v, int size);
int kernel_memcmp(const void *dest1, const void *dest2, int size);

void kernel_sprintf(char *buf, const char *formate, ...);
void kernel_vsprintf(char *buf, const char *formate, va_list args);



#endif