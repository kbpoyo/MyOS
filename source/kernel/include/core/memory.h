/**
 * @file memory.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义内存分配相关数据结构
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "common/types.h"
#include "tools/bitmap.h"
#include "ipc/mutex.h"
#include "common/boot_info.h"

//实模式下的1mb内存空间
#define MEM_EXT_START (1024*1024)
#define MEM_PAGE_SIZE 4096
//实模式下1mb空间中拓展bios数据区的首地址，紧邻可用数据区
#define MEM_EBDA_START 0x9fc00

//内存分配对象
typedef struct _addr_alloc_t {
    mutex_t mutex;      //分配内存时进行临界资源管理
    bitmap_t bitmap;    //管理内存页的位图结构
    uint32_t start;     //管理内存区域的起始地址
    uint32_t size;      //内存区域的大小
    uint32_t page_size; //页的大小

}addr_alloc_t;

void memory_init(boot_info_t *boot_info);




#endif
