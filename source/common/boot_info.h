/**
 * @file boot_info.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  初始化系统信息
 * @versioinin0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include "types.h"

//内存中可用内存是不连续的，该宏指定需要检测的可用内存块的数量
#define BOOT_RAM_REGION_MAX 10


/**
 * @brief  mbr记录系统信息
 */
typedef struct _boot_info_t_ {
    struct {
        uint32_t start; //内存块起始地址
        uint32_t size;  //内存块大小
    }ram_region_cfg[BOOT_RAM_REGION_MAX]; //记录有效的内存块的数组
    
    int ram_region_count;//记录有效内存卡数组的实际大小

}_boot_info_t_;

//磁盘分区默认大小为512字节
#define SECTOR_SIZE 512


#endif