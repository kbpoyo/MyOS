/**
 * @file bitmap.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义位图数据结构
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef BITMAP_H
#define BITMAP_H

#include "common/types.h"

typedef struct  _bitmap_t {
    int bit_count;  //内存分页数量
    uint8_t *bits;  //位图的自己数组
}bitmap_t;

void bitmap_init(bitmap_t *bitmap, uint8_t *bits, int count, int init_bit);
int bitmap_get_bit(bitmap_t *bitmap, int index);
void bitmap_set_bit(bitmap_t *bitmap, int index, int count, int bit);
int bitmap_is_set(bitmap_t *bitmap, int index);
int bitmap_alloc_nbits(bitmap_t *bitmap, int bit, int count);

#endif