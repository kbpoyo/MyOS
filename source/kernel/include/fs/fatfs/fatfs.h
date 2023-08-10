/**
 * @file fatfs.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief fat16文件系统
 * @version 0.1
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FATFS_H
#define FATFS_H

#include "common/types.h"

#pragma pack(1)

//dbr区域结构，存储了fat16文件系统的配置信息
typedef struct _dbr_t {
    //跳转代码 区域
    uint8_t BS_jmpBoot[3];  //忽略，3字节的跳转代码
    uint8_t BS_OEMName[8];  //忽略，8字节的操作系统名称

    //BISO Parameter block 区域
    uint16_t BPB_BytsPerSec;    //每扇区字节数
    //每簇的扇区数，fat系统会以簇为单位存储文件
    //一簇为2的n次方个扇区
    uint8_t BPB_SecPerClus; 
    uint16_t BPB_RsvdSecCnt;    //保留区中保留扇区的数目
    uint8_t BPB_NumFATs;        //FAT表项的个数，一般固定为2
    uint16_t BPB_RootEntCnt;    //根目录区域中的目录项个数
    uint16_t BPB_TotSec16;      //FAT16的总扇区数
    uint8_t BPB_Media;
    uint16_t BPB_FATSz16;       //FAT表项的大小
    uint16_t BPB_SecPerTrk;     //忽略，CHS模式下的每磁忽略扇区数
    uint16_t BPB_NumHeads;      //忽略，CHS模式下的磁头数
    uint16_t BPB_HiddSec;       //忽略，FAT分区之前隐藏的扇区数
    uint32_t BPB_TotSec32;      //忽略，FAT32的总扇区数

    //FAT配置数据区
    uint8_t BS_drvNum;          //忽略，磁盘驱动器参数
    uint8_t BS_Reserved;        //忽略，保留
    uint8_t BS_BootSig;         //忽略，拓展引导标记，用于指明此后的三个区域可用
    uint32_t BS_VollD;          //忽略，卷标序号
    uint8_t BS_VolLab[11];      //忽略，磁盘卷标
    //忽略，存放"FAT 16" "FAT 12" "FAT32" "NOTE"
    //但并不是用来确定文件系统类型的
    uint8_t BS_FilSysType[8];  

}dbr_t;

#pragma pack()


//fat表结构
typedef struct _fat_t {
    uint32_t tbl_start; //表的起始地址
    uint32_t tbl_cnt;   //表数量
    uint32_t tbl_sectors;   //表

    uint32_t bytes_per_sector;  //每个扇区的字节数
    uint32_t sec_per_cluster;   //每簇的扇区数量
    uint32_t root_start;    //根目录区的起始地址
    uint32_t root_ent_cnt;  //根目录区的项数
    uint32_t data_start;    //文件数据区域的起始地址

    struct _fs_t *fs;   //该分区所属的文件系统
        
}fat_t;

#endif
