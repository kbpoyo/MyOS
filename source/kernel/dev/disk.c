/**
 * @file disk.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 磁盘
 * @version 0.1
 * @date 2023-08-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/disk.h"
#include "tools/log.h"
#include "tools/klib.h"
#include "common/cpu_instr.h"


//系统磁盘表
static disk_t disk_table[DISK_CNT];

/**
 * @brief 向磁盘指令io端口发送指令
 * 
 * @param disk 
 * @param start_sector 
 * @param sector_count 
 * @param cmd 
 */
static void disk_send_cmd(disk_t *disk, uint32_t start_sector, uint32_t sector_count, int cmd) {
    //1.LBA模式下指定磁盘的驱动器号，即master或者slave
    outb(DISK_DRIVE(disk), DISK_DRIVE_BASE + disk->drive);
    
    //2.指定扇区数的高八位和起始扇区即LBA值的高24位
    outb(DISK_SECTOR_COUNT(disk), (uint8_t)(sector_count >> 8));
    //6字节LBA值，先初始化第456个字节
    //暂时只用到了4个字节的LBA值, 所以第5, 6个字节置0即可
    outb(DISK_LBA_LO(disk), (uint8_t)(start_sector >> 24)); //LBA4
    outb(DISK_LBA_MID(disk), 0x00);                     // LBA5
    outb(DISK_LBA_HI(disk), 0x00);                     // LBA6

    //3.指定读取扇区的低8位和起始扇区即LBA值的低24位
    outb(DISK_SECTOR_COUNT(disk), (uint8_t)(sector_count >> 0));
    outb(DISK_LBA_LO(disk), (uint8_t)(start_sector >> 0));   //LBA1
    outb(DISK_LBA_MID(disk), (uint8_t)(start_sector >> 8)); //LBA2
    outb(DISK_LBA_HI(disk), (uint8_t)(start_sector >> 16)); //LBA3

    //4.对指定的区域执行cmd指令操作
    outb(DISK_CMD(disk), cmd);
}

/**
 * @brief 初始化系统磁盘信息
 * 
 */
void disk_init(void) {
    log_printf("Check disk...\n");
    
    kernel_memset(disk_table, 0, sizeof(disk_table));
    //遍历并初始化化primary信道上的磁盘信息
    for (int i = 0; i < DISK_PER_CHANNEL; ++i) {
        disk_t *disk = disk_table + i;
        kernel_sprintf(disk->name, "sd%c", i + 'a');
        disk->drive = i == 0 ? DISK_MASTER : DISK_SLAVE;
        disk->port_base = IOBASE_PRIMARY;

        int err = identify_disk(disk);
        if (err == 0) {
            print_disk_info(disk);
        }
    }
    
}