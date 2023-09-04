/**
 * @file bitmap.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义位图相关操作
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/bitmap.h"
#include "tools/assert.h"
#include "tools/klib.h"


/**
 * @brief  向上取整获取位图数组中有多少字节
 * 
 * @param bit_count  位图数组拥有的bit位数
 * @return int 向上取整得到的字节数
 */
int bitmap_byte_count(int bit_count) {
    return (bit_count + 8 - 1) / 8;
}

/**
 * @brief  初始化位图数据结构
 * 
 * @param bitmap 需要初始化的位图
 * @param bits 位图所包含的位图数组的起始地址
 * @param count 页的数量
 * @param init_bit 将位图数组的每一位设置为init_bit
 */
void bitmap_init(bitmap_t *bitmap, uint8_t *bits, int count, int init_bit) {
    ASSERT(bitmap != (bitmap_t*)0);
    ASSERT(bits != (uint8_t*)0);

    bitmap->bit_count = count;
    bitmap->bits = bits;

    //计算该位图需要多少字节
    int bytes = bitmap_byte_count(bitmap->bit_count);
    
    kernel_memset(bitmap->bits, init_bit ? 0xff : 0, bytes);

}

/**
 * @brief  获取bitmap中第index位的bit值,index从0开始
 * 
 * @param bitmap 
 * @param index 
 * @return uint8_t 
 */
uint8_t bitmap_get_bit(bitmap_t *bitmap, int index) {
    ASSERT(bitmap != (bitmap_t*)0);
    ASSERT(index >= 0);

    return (bitmap->bits[index / 8] & (1 << (index % 8)));
}

/**
 * @brief  将bitmap中的位图数组从index位开始一共count位，设置为bit
 * 
 * @param bitmap 
 * @param index 
 * @param count 
 * @param bit 
 */
void bitmap_set_bit(bitmap_t *bitmap, int index, int count, int bit) {
    ASSERT(bitmap != (bitmap_t*)0);
    ASSERT(index >= 0 && count >= 0);
    for (int i = 0; i < count && index < bitmap->bit_count; ++i, ++index) {
        if (bit) {
            bitmap->bits[index / 8] |= (1 << (index % 8));
        } else {
            bitmap->bits[index / 8] &= ~(1 << (index % 8));
        }
    }
}

/**
 * @brief  返回bitmap中index位是否被置1
 * 
 * @param bitmap 
 * @param index 
 * @return int 
 */
int bitmap_is_set(bitmap_t *bitmap, int index) {
    return bitmap_get_bit(bitmap, index) ? 1 : 0;
}


/**
 * @brief  在bitmap中分配一块大小为count个位的空间
 * 
 * @param bitmap 
 * @param bit 当某一位的值为bit时表示该位空闲，可供分配
 * @param count 
 * @return int 
 */
int bitmap_alloc_nbits(bitmap_t *bitmap, int bit, int count) {
    ASSERT(bitmap != (bitmap_t*)0);
    ASSERT(count >= 0);

    int search_index = 0;
    int ok_index = -1;
    while (search_index < bitmap->bit_count) {
        //确定可分配空间的起始索引
        if (bitmap_get_bit(bitmap, search_index) != bit) {
            search_index++;
            continue;
        }

        //记录可分配空间的起始索引
        ok_index = search_index++;

        //判断该空间是否大小满足要求
        for (int i = 1; i < count && search_index < bitmap->bit_count; ++i) {
            //空间大小不满足要求, 分配失败
            if (bitmap_get_bit(bitmap, search_index++) != bit) {
                ok_index = -1;
                break;
            }
        }

        if (ok_index != -1) {//分配成功
            //将该片空间标记为已分配状态, 并返回起始索引
            bitmap_set_bit(bitmap, ok_index, count, ~bit);
            return ok_index;
        }
    }
    
    //遍历完整个位图也没有满足要求的空间则返回-1
    return -1;

}