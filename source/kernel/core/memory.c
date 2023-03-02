/**
 * @file memory.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  实现内存管理的相关方法
 * @version 0.1
 * @date 2023-03-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "core/memory.h"
#include "tools/log.h"

/**
 * @brief  初始化内存分配对象
 *
 * @param alloc 内存分配对象
 * @param bits 内存分配对象所管理的位图数组的起始地址
 * @param start 管理内存的起始地址
 * @param size 管理内存的大小
 * @param page_size 管理的内存页的大小
 */
static void addr_alloc_init(addr_alloc_t *alloc, uint8_t *bits, uint32_t start,
                            uint32_t size, uint32_t page_size) {
  mutex_init(&alloc->mutex);
  alloc->start = start;
  alloc->size = size;
  alloc->page_size = page_size;
  bitmap_init(&alloc->bitmap, bits, alloc->size / alloc->page_size, 0);
}

/**
 * @brief  申请连续的内存页
 *
 * @param alloc
 * @param page_count 申请页的数量
 * @return uint32_t 申请的第一个页的起始地址
 */
static uint32_t addr_alloc_page(addr_alloc_t *alloc, int page_count) {
  uint32_t addr = 0;  // 记录分配的页的起始地址

  mutex_lock(&alloc->mutex);

  // 在位图中取连续的page_count个页进行分配
  int page_index = bitmap_alloc_nbits(&alloc->bitmap, 0, page_count);
  if (page_count >= 0) {
    // 计算出申请到的第一个页的起始地址
    addr = alloc->start + page_index * alloc->page_size;
  }

  mutex_unlock(&alloc->mutex);

  return addr;
}

/**
 * @brief  释放连续内存页
 *
 * @param alloc
 * @param addr 第一个内存页的起始地址
 * @param page_count 内存页的数量
 */
static void addr_free_page(addr_alloc_t *alloc, uint32_t addr, int page_count) {
  mutex_lock(&alloc->mutex);

  // 计算出第一个页在位图中的索引
  int page_index = (addr - alloc->start) / alloc->page_size;

  bitmap_set_bit(&alloc->bitmap, page_index, page_count, 0);

  mutex_unlock(&alloc->mutex);
}

/**
 * @brief  初始化化内存
 *
 * @param boot_info cpu在实模式下检测到的可用内存对象
 */
void memory_init(boot_info_t *boot_info) {
    
    addr_alloc_t addr_alloc;    //内存分配对象
    uint8_t bits[8];    //提供给内存分配对象的位图缓冲区，8个字节供支持64个页的管理

    //对起始地址为0x1000，大小为64个4kb的内存页的空间进行管理初始化
    addr_alloc_init(&addr_alloc, bits, 0x1000, 64 * 4096, 4096);

    for (int i = 0; i < 32; ++i) {
        uint32_t addr = addr_alloc_page(&addr_alloc, 2);
        log_printf("alloc addr: 0x%x, bitmap: 0b%b", addr, bits[2*i / 8]);
    }



    uint32_t addr = 0x1000;
    for (int i = 0; i < 32; ++i) {
        addr_free_page(&addr_alloc, addr, 2);
        log_printf("free addr: 0x%x, bitmap: 0b%b", addr, bits[2*i / 8]);
        addr += 2*4096;
    }

}