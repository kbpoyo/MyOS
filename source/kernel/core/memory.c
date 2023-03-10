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
#include "tools/klib.h"
#include "tools/assert.h"
#include "cpu/mmu.h"

//定义全局内存页分配对象
static addr_alloc_t paddr_alloc;

//1.声明页目录表结构，并且使该页目录的起始地址按4kb对齐
//页目录项的高10位为页的物理地址位，及1024个子页表
static pde_t kernel_page_dir_table[PDE_CNT] __attribute__((aligned(MEM_PAGE_SIZE)));



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
 * @brief  打印1m以内内存的可用空间
 * 
 * @param boot_info 
 */
static void show_mem_info(boot_info_t *boot_info) {
    log_printf("mem region:");
    for (int i = 0; i < boot_info->ram_region_count; ++i) {
        log_printf("[%d]: 0x%x - 0x%x", i, boot_info->ram_region_cfg[i].start, boot_info->ram_region_cfg[i].size);
    }

    log_printf("");
}

/**
 * @brief  计算总的可用内存空间大小
 * 
 * @param boot_info 
 * @return uint32_t 
 */
static uint32_t total_mem_size(boot_info_t *boot_info) {
    uint32_t mem_size = 0;
    for (int i = 0; i < boot_info->ram_region_count; ++i) {
        mem_size += boot_info->ram_region_cfg[i].size;
    }

    return mem_size;
}

//TODO:编写函数注释
void create_kernal_table(void) {
  
  //声明内核只读段的起始与结束地址和数据段的起始地址
  extern char s_text, e_text, s_data;

  static memory_map_t kernal_map[] = {
    {0, &s_text, 0, 0},                             //低64kb的空间映射关系，即0x10000(内核起始地址)以下部分的空间
    {&s_text, &e_text, &s_text, 0},                 //只读段的映射关系(内核.text和.rodata段)
    {&s_data, (void*)MEM_EBDA_START, &s_data, 0}    //可读写段的映射关系，一直到bios的拓展数据区(内核.data与.bss段再加上剩余的可用数据区域)

  };

  for (int i = 0; i < sizeof(kernal_map) / sizeof(kernal_map[0]); ++i) {
    memory_map_t *map = kernal_map + i;

    //将虚拟地址的起始地址按页大小4kb对齐，为了不丢失原有的虚拟地址空间，所以向下对齐vstart
    uint32_t vstart = down2((uint32_t)map->vstart, MEM_PAGE_SIZE);
    //将虚拟地址的结束地址按页大小4kb对齐, 为了不丢失原有的虚拟地址空间，所以向上对齐vend
    uint32_t vend = up2((uint32_t)map->vend, MEM_PAGE_SIZE);
    //计算该虚拟空间需要的页数
    int page_count = (vend - vstart) / MEM_PAGE_SIZE;


  }
}


/**
 * @brief  初始化化内存
 *
 * @param boot_info cpu在实模式下检测到的可用内存对象
 */
void memory_init(boot_info_t *boot_info) {

  
    //声明紧邻在内核bss段后面的空间地址，用于存储位图，该变量定义在kernel.lds中
    extern char mem_free_start;

    log_printf("memory init");

    log_printf("mem_free_start: %d", mem_free_start);

    show_mem_info(boot_info);
    
    //去除保护模式下1mb大小后可用的内存空间大小
    uint32_t mem_up1MB_free = total_mem_size(boot_info) - MEM_EXT_START;

    //将可用空间大小下调到页大小的整数倍
    mem_up1MB_free = down2(mem_up1MB_free, MEM_PAGE_SIZE);
    
    log_printf("free memory: 0x%x, size: 0x%x", MEM_EXT_START, mem_up1MB_free);

    //mem_free_start被分配的地址在链接文件中定义，紧邻着内核的.bss段
    uint8_t *mem_free = (uint8_t*)&mem_free_start;

    //用paddr_alloc，内存页分配对象管理1mb以上的所有空闲空间，页大小为MEM_PAGE_SIZE=4kb
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_up1MB_free, MEM_PAGE_SIZE);

    //跳过存储位图的内存区域, 位图的每一位表示一个页，计算位图所站的字节数即可跳过该区域
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);  

    //判断mem_free是否已越过可用数据区
    ASSERT(mem_free < (uint8_t*)MEM_EBDA_START);
    
    //创建内核的页表映射
    create_kernal_table();

    //设置内核的页目录表到CR3寄存器，并开启分页机制
    mmu_set_page_dir_table((uint32_t)kernel_page_dir_table)
}