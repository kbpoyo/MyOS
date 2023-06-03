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

/**
 * @brief  拷贝字符串
 *
 * @param dest 目标字符串
 * @param src  源字符串
 */
void kernel_strcpy(char *dest, const char *src) {
  // 1.判断是否为NULL
  if (!dest || !src) return;

  // 2.拷贝 src '\0' 以前的部分到 dest中
  // TODO:完成（访问未初始化的内存）的异常中断后就只判断src是否读到'\0'
  while (*dest && *src) {
    *(dest++) = *(src++);
  }

  // 3.添加结束符
  *dest = '\0';
}

/**
 * @brief  按大小拷贝字符串
 *
 * @param dest 目的字符串
 * @param src 源字符串
 * @param size 拷贝字符串的长度，包含'\0'
 */
void kernel_strncpy(char *dest, const char *src, int size) {
  // 1.判断是否需要拷贝
  if (!dest || !src || !size) return;

  // 2.size的大小包含了'\0'，所以要预留一个空间给'\0'，只能拷贝 size - 1个字符
  while ((size-- > 1) && (*src)) {
    *(dest++) = *(src++);
  }

  // 3.添加结束符
  *dest = '\0';
}

/**
 * @brief  比较字符串
 *
 * @param s1 字符串1
 * @param s2 字符串2
 * @param size 比较前比较的子串长度，从下标0开始到(size - 1)
 *
 * @return int ==: 0, > : 1, < : -1
 */
int kernel_strncmp(const char *s1, const char *s2, int size) {
  if (!s1 || !s2) return 0;

  while (*s1 && *s2 && --size && *s1 == *s2) {
    ++s1;
    ++s2;
  }

  if (*s1 > *s2)
    return 1;
  else if (*s1 < *s1)
    return -1;
  else
    return 0;
}

/**
 * @brief  获取字符串str的长度, 不含'\0'
 *
 * @param str
 * @return int
 */
int kernel_strlen(const char *str) {
  if (!str) return 0;

  int len = 0;
  while (*(str++)) len++;

  return len;
}

/**
 * @brief  内存复制
 *
 * @param dest 目的内存的起始地址
 * @param src 原内存的起始地址
 * @param size 复制的内存字节大小
 */
void kernel_memcpy(void *dest, const void *src, int size) {
  if (!dest || !src || !size) return;

  uint8_t *d = (uint8_t *)dest;
  uint8_t *s = (uint8_t *)src;

  while (size--) {
    *(d++) = *(s++);
  }
}

/**
 * @brief  对内存区域每一个字节赋值置
 *
 * @param dest 区域起始地址
 * @param v 每个字节的值
 * @param size 赋值的内存的大小
 */
void kernel_memset(void *dest, uint8_t v, int size) {
  if (!dest || !size) return;

  uint8_t *d = (uint8_t *)dest;

  while (size--) {
    *(d++) = v;
  }
}

/**
 * @brief  按按内存区域从低地址到高地址逐字节比较大小
 *
 * @param d1 区域1的起始地址
 * @param d2 区域2的起始地址
 * @param size 比较的字节数量
 * @return int ==:0, >:1, <:-1
 */
int kernel_memcmp(const void *dest1, const void *dest2, int size) {
  if (!dest1 || !dest2 || !size) return 0;

  uint8_t *d1 = (uint8_t *)dest1;
  uint8_t *d2 = (uint8_t *)dest2;

  while (--size && *d1 == *d2) {
    d1++;
    d2++;
  }

  if (*d1 > *d2)
    return 1;
  else if (*d1 < *d2)
    return -1;
  else
    return 0;
}

void kernel_sprintf(char *buf, const char *formate, ...) {
  // 获取可变参数并将其格式化到缓冲区中
  va_list args;
  va_start(args, formate);
  kernel_vsprintf(buf, formate, args);
  va_end(args);
}

/**
 * @brief  格式化字符串
 *
 * @param buf
 * @param formate
 * @param args
 */
void kernel_vsprintf(char *buf, const char *formate, va_list args) {
  // 定义状态机，包含两种状态
  enum { NORMAL, READ_FMT } state = NORMAL;

  char *curr = buf;

  char ch;

  while ((ch = *(formate++)) != '\0') {
    switch (state) {
      case NORMAL:
        if (ch == '%')
          state = READ_FMT;
        else
          *(curr++) = ch;
        break;
      case READ_FMT:
        if (ch == 's') {
          const char *str = va_arg(args, char *);
          int len = kernel_strlen(str);
          while (len--) {
            *(curr++) = *(str++);
          }
        } else if (ch == 'd') {
          const int num = va_arg(args, int);
          kernel_itoa(curr, num, 10);
          curr += kernel_strlen(curr);
        } else if (ch == 'x' || ch == 'X') {
          const int num = va_arg(args, int);
          kernel_itoa(curr, num, 16);
          curr += kernel_strlen(curr);
        } else if (ch == 'b') {
          const int num = va_arg(args, int);
          kernel_itoa(curr, num, 2);
          curr += kernel_strlen(curr);
        } else if (ch == 'c') {
          char c = va_arg(args, int);
          *(curr++) = c;
        }
        state = NORMAL;
        break;
      default:
        break;
    }
  }
}

/**
 * @brief  将整数num转换为字符串放入缓冲区buf中
 *
 * @param buf 缓冲区
 * @param num 整数
 * @param base 转换的进制规则
 */
void kernel_itoa(char *buf, int num, int base) {
  char *p = buf;

  if (base != 2 && base != 8 && base != 10 && base != 16) {
    *buf = '\0';
    return;
  }

  uint32_t u_num = 0;

  if (base == 10) {
    u_num = num;
    if (num < 0) {
      *(p++) = '-';
      u_num = -1 * num;
    } else if (num == 0) {
      *(p++) = '0';
      *p = '\0';
      return;
    }
  } else {
    u_num = *((uint32_t *)&num);
  }

  static const char *num_to_char = {"0123456789abcdef"};
  char arr[128];
  int len = 0;
  while (u_num > 0) {
    arr[len++] = num_to_char[u_num % base];
    u_num /= base;
  }

  for (int i = len - 1; i >= 0; --i) {
    *(p++) = arr[i];
  }
  *p = '\0';
}

/**
 * @brief 计算字符串数组str中字符串的个数
 *
 * @param str
 * @return int
 */
int strings_count(char *const *strs) {
  int count = 0;
  if (strs) {
    while (*(strs++)) {
      count++;
    }
  }

  return count;
}

/**
 * @brief 根据文件路径获取文件名
 *
 * @param path
 * @return char*
 */
const char *get_file_name(const char *path) {
  const char *s = path;
  while (*s != '\0') {
    s++;
  }

  while ((*s != '/') && (*s != '\\') && (s >= path)) {
    s--;
  }

  return s + 1;
}