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
#include "ipc/mutex.h"
#include "ipc/sem.h"

#define DISK_NAME_SIZE          32
#define PART_NAME_SIZE          32 
#define DISK_PRIMARY_PART_CNT   (4 + 1) //每个磁盘的最大分区数
#define DISK_CNT                2   //系统支持的磁盘数量
#define DISK_PER_CHANNEL        2   //每个信道上的磁盘数量
#define IOBASE_PRIMARY          0x1F0   //Primary信道上的磁盘的起始io端口


//磁盘各个寄存器的端口
#define DISK_DATA(disk)             (disk->port_base + 0)   //计算数据寄存器的地址
#define DISK_ERROR(disk)            (disk->port_base + 1)   //错误寄存器
#define DISK_SECTOR_COUNT(disk)     (disk->port_base + 2)   //扇区数量寄存器
#define DISK_LBA_LO(disk)           (disk->port_base + 3)   //LBA寄存器
#define DISK_LBA_MID(disk)          (disk->port_base + 4)   //LBA寄存器
#define DISK_LBA_HI(disk)           (disk->port_base + 5)   //LBA寄存器
#define DISK_DRIVE(disk)           (disk->port_base + 6)   //磁盘或磁头寄存器
#define DISK_STATUS(disk)           (disk->port_base + 7)   //状态寄存器
#define DISK_CMD(disk)              (disk->port_base + 7)   //控制指令寄存器

//磁盘DIRVE寄存器的缺省值, LBA模式下不需要指定磁头
//寄存器的7~5位恒为1, 第4位为驱动器位，1表示从驱动器，0表示主驱动器
#define DISK_DRIVE_BASE     0xE0

//磁盘操作指令
#define DISK_CMD_IDENTIFY   0xEC    //识别磁盘
#define DISK_CMD_READ       0x24    //读磁盘
#define DISK_CMD_WRITE      0x34    //写磁盘

//磁盘状态
#define DISK_STATUS_ERR     (1 << 0)//磁盘执行指令时发生错误
#define DISK_STATUS_DRQ     (1 << 3)//准备好接受数据或者输出数据
#define DISK_STATUS_DF      (1 << 5)//驱动错误
#define DISK_STATUS_BUSY    (1 << 7)//磁盘忙碌

//primary信道上的磁盘分区表数量 
#define MBR_PRIMARY_PART_NR 4


#pragma pack(1)
//分区表结构
typedef struct _part_item_t {
    uint8_t boot_active;    //扇区是否激活， 0 = no, 0x80 = active
    uint8_t start_header;   //CHS模式下的起始磁头
    uint16_t start_sector:6;    //CHS模式下的起始扇区
    uint16_t start_cylinder:10; //CHS模式下的起始柱面
    uint8_t system_id;
    uint8_t end_header; //CHS模式下的结束磁头
    uint16_t end_sector:6;   //CHS模式下的结束扇区
    uint16_t end_cylinder:10;    //CHS模式下的结束面
    uint32_t relative_sector;  //相对扇区号，也就是LBA模式下的起始扇区号
    uint32_t total_sectors; //分区所拥有的扇区总数，供LBA模式使用
}part_item_t;


//磁盘0扇区mbr的结构
typedef struct _mbr_t {
    uint8_t code[446];  //446字节存放引导代码
    part_item_t part_item[MBR_PRIMARY_PART_NR]; //64字节的分区表数组，数量为4
    uint8_t boot_sig[2];//两字节的mbr标志，0x55,0xaa
}mbr_t;

#pragma pack()



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
    int total_sectors;   //分区所拥有的扇区数量
}partinfo_t;


//磁盘结构体，描述磁盘信息
typedef struct _disk_t {
    char name[DISK_NAME_SIZE];

    //磁盘类型，主磁盘和从磁盘，即磁盘所在的驱动器号
    //驱动器号为DRIVE寄存器的第4位，通过写入DRIVE寄存器指定
    enum {
        DISK_MASTER = (0 << 4),
        DISK_SLAVE = (1 << 4),
    }drive;

    uint16_t port_base;

    int sector_size;    //扇区大小
    int sector_count;   //扇区数量
    partinfo_t partinfo[DISK_PRIMARY_PART_CNT]; //分区结构数组

    mutex_t *mutex;   //磁盘互斥锁，确保磁盘io操作的原子性
    sem_t *op_sem;  //磁盘操作信号量，等待磁盘数据就绪，节省磁盘io时间
}disk_t;

void disk_init(void);

void exception_handler_primary_disk(void);

#endif
