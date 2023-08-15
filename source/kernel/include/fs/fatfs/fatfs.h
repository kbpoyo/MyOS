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

//清空簇链关系时,该簇号标志此FAT表项空闲
#define CLUSTER_FAT_FREE        0x0
//标志该簇对应的号码无效
#define FAT_CLUSTER_INVALID     0xfff8
//标志该root_entry是空闲的
#define DIRITEM_NAEM_FREE       0xE5
//标志该root_entry末尾项
#define DIRITEM_NAME_END        0x00

#define DIRITEM_ATTR_READ_ONLY  0x1     //此目录项对应一个只读文件
#define DIRITEM_ATTR_HIDDEN     0x2     //此目录项对应一个隐藏文件
#define DIRITEM_ATTR_SYSTEM     0x4     //此目录项对应一个系统文件
#define DIRITEM_ATTR_VOLUME_ID  0x8
#define DIRITEM_ATTR_DIRECTORY  0x10    //此目录项对应一个目录文件
#define DIRITEM_ATTR_ARCHIVE    0x20
#define DIRITEM_ATTR_LONG_NAME  0xF //长文件名类型的文件会用两个目录项来记录
                                    //我直接忽略这种文件

#define SFN_LEN                 11// sfn系统文件名长

//根目录区的目录项结构
typedef struct _diritem_t {
        uint8_t DIR_Name[11];
        uint8_t DIR_Attr;   //文件属性
        uint8_t DIR_NTRes;
        uint8_t DIR_CrtTimeTeenth;
        uint16_t DIR_CrtTime;   //文件创建时间
        uint16_t DIR_CrtDate;   //文件创建日期
        uint16_t DIR_LastAccDate;   //文件最后访问日期
        uint16_t DIR_FstClusHI; //文件起始簇号的高16位，FAT12/16此位为0
        uint16_t DIR_WrtTime;   //文件最后写的时间
        uint16_t DIR_WrtDate;   //文件最后写的日期
        uint16_t DIR_FstClusLo; //文件起始簇号的低16位
        uint32_t DIR_FileSize;//文件大小
        
}diritem_t;

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
    uint16_t BPB_TotSec16;      //FAT16文件系统的总扇区数
    uint8_t BPB_Media;
    uint16_t BPB_FATSz16;       //FAT表的总扇区数
    uint16_t BPB_SecPerTrk;     //忽略，CHS模式下的每磁忽略扇区数
    uint16_t BPB_NumHeads;      //忽略，CHS模式下的磁头数
    uint32_t BPB_HiddSec;       //忽略，FAT分区之前隐藏的扇区数
    uint32_t BPB_TotSec32;      //忽略，FAT32的总扇区数

    //FAT配置数据区
    uint8_t BS_drvNum;          //忽略，磁盘驱动器参数
    uint8_t BS_Reserved;        //忽略，保留
    uint8_t BS_BootSig;         //忽略，拓展引导标记，用于指明此后的三个区域可用
    uint32_t BS_VollD;          //忽略，卷标序号
    uint8_t BS_VolLab[11];      //忽略，磁盘卷标
    //忽略，存放"FAT16" "FAT12" "FAT32" "NOTE"
    //但并不是用来确定文件系统类型的
    uint8_t BS_FilSysType[8];  

}dbr_t;

#pragma pack()


//fat表结构
typedef struct _fat_t {
    uint32_t tbl_start_sector; //FAT表的起始地址
    uint32_t tbl_cnt;   //FAT表数量
    uint32_t tbl_sectors;   //FAT表所占的扇区数量

    uint32_t bytes_per_sector;  //每个扇区的字节数
    uint32_t sec_per_cluster;   //每簇的扇区数量
    uint32_t root_start_sector;    //根目录区的起始地址
    uint32_t root_ent_cnt;  //根目录区的项数,每一项占32个字节
    uint32_t data_start_sector;    //文件数据区域的起始地址
    uint32_t cluster_bytes_size;    //一簇的字节大小

    uint32_t curr_sector;   //fat_buffer当前缓存的扇区号
    uint8_t *fat_buffer;    //fat表结构的缓冲区，可用于存放读取到内存的dbr区域
    
    struct _fs_t *fs;   //该分区所属的文件系统

        
}fat_t;

typedef uint16_t cluster_t;

#endif
