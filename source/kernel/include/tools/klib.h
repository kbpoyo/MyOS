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

/**
 * @brief  将size向下转换为bound的整数倍
 * 
 * @param size 
 * @param bound 一般取2的n次方
 * @return uint32_t 
 */
static inline uint32_t down2(uint32_t size, uint32_t bound) {
    return size & ~(bound - 1);  
}

/**
 * @brief  将size向上转换为bound的整数倍
 * 
 * @param size 
 * @param bound 一般取2的n次方
 * @return uint32_t 
 */
static inline uint32_t up2(uint32_t size, uint32_t bound) {
    return (size + bound - 1) & ~(bound - 1);
}

void kernel_strcpy(char *dest, const char *src);
void kernel_strncpy(char* dest, const char *src, int size);
int  kernel_strncmp(const char *s1, const char *s2, int size);
void kernel_itoa(char *buf, int num, int base);
int kernel_strlen(const char *str);

void kernel_memcpy(void *dest, const void *src, int size);
void kernel_memset(void *dest, uint8_t v, int size);
int kernel_memcmp(const void *dest1, const void *dest2, int size);

void kernel_sprintf(char *buf, const char *formate, ...);
void kernel_vsprintf(char *buf, const char *formate, va_list args);

int strings_count(char * const *str);
char* get_file_name(const char *ame);


#endif