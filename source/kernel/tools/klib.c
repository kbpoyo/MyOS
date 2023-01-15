/**
 * @file klib.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  自键一些便捷的c库函数
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/klib.h"

void kernel_strcpy(char *dest, const char *src) {
    //1.判断是否为NULL
    if (!dest || !src) return;

    //2.拷贝 src '\0' 以前的部分到 dest中
    //TODO:完成（访问未初始化的内存）的异常中断后就只判断src是否读到'\0'
    while (*dest && *src) {
        *(dest++) = *(src++);
    }

    //3.添加结束符
    *dest = '\0';
    
}

void kernel_strncpy(char* dest, const char *src, int size) {
    //1.判断是否需要拷贝
    if (!dest || !src || !size) return;

    //2.size的大小包含了'\0'，所以要预留一个空间给'\0'，只能拷贝 size - 1个字符
    while ((size-- > 1) && (*src)) {
        *(dest++) = *(src++);
    }

    //3.添加结束符
    *dest = '\0';
}

void kernel_strncmp(const char *s1, const char *s2, int size);

void kernel_memcpy(void *dest, void *src, int size);
void kernel_memset(void *dest, uint8_t v, int size);
int kernel_memcmp(void *d1, void *d2, int size);