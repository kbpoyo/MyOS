/**
 * @file disk.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 磁盘设备
 * @version 0.1
 * @date 2023-08-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DISK_H
#define DISK_H

#include "common/types.h"

#define DISK_NAME_SIZE 32
#define PART_NAME_SIZE 32 
#define DISK_PRIMARY_PART_CNT   (4 + 1) //每个磁盘的最大分区数
#define DISK_CNT    2   //系统支持的磁盘数量


struct _disk_t;

//分区结构体，描述分区信息
typedef struct _partinfo_t {
    char name[PART_NAME_SIZE];
    struct _disk_t *disk;   //分区所属磁盘

    //分区类型枚举
    enum {
        FS_INVALID = 0x00, //无效分区
        FS_FAT16_0 = 0x6,   //fat16分区，类型1
        FS_FAT16_1 = 0xE,   //fat16分区，类型2

    }type;

    int start_sector;   //分区起始扇区
    int total_sector;   //分区所拥有的扇区数量
}partinfo_t;


//磁盘结构体，描述磁盘信息
typedef struct _disk_t {
    char name[DISK_NAME_SIZE];
    int sector_size;    //扇区大小
    int sector_count;   //扇区数量
    partinfo_t partinfo[DISK_PRIMARY_PART_CNT]; //分区结构数组
}disk_t;

void disk_init(void);

#endif
