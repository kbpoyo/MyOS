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

#include "common/cpu_instr.h"
#include "tools/klib.h"
#include "tools/log.h"
#include "common/boot_info.h"

// 系统磁盘表
static disk_t disk_table[DISK_CNT];

/**
 * @brief 向磁盘指令io端口发送指令
 *
 * @param disk
 * @param start_sector
 * @param sector_count
 * @param cmd
 */
static void disk_send_cmd(disk_t *disk, uint32_t start_sector,
                          uint32_t sector_count, int cmd) {
  // 1.LBA模式下指定磁盘的驱动器号，即master或者slave
  outb(DISK_DRIVE(disk), DISK_DRIVE_BASE | disk->drive);

  // 2.指定扇区数的高八位和起始扇区即LBA值的高24位
  outb(DISK_SECTOR_COUNT(disk), (uint8_t)(sector_count >> 8));
  // 6字节LBA值，先初始化第456个字节
  // 暂时只用到了4个字节的LBA值, 所以第5, 6个字节置0即可
  outb(DISK_LBA_LO(disk), (uint8_t)(start_sector >> 24));  // LBA4
  outb(DISK_LBA_MID(disk), 0x00);                          // LBA5
  outb(DISK_LBA_HI(disk), 0x00);                           // LBA6

  // 3.指定读取扇区的低8位和起始扇区即LBA值的低24位
  outb(DISK_SECTOR_COUNT(disk), (uint8_t)(sector_count >> 0));
  outb(DISK_LBA_LO(disk), (uint8_t)(start_sector >> 0));   // LBA1
  outb(DISK_LBA_MID(disk), (uint8_t)(start_sector >> 8));  // LBA2
  outb(DISK_LBA_HI(disk), (uint8_t)(start_sector >> 16));  // LBA3

  // 4.对指定的区域执行cmd指令操作
  outb(DISK_CMD(disk), cmd);
}

/**
 * @brief 从磁盘disk中读取size大小的数据到buf中
 *
 * @param disk
 * @param buf
 * @param size
 */
static void disk_read_data(disk_t *disk, void *buf, int size) {
  uint16_t *c = (uint16_t *)buf;

  for (int i = 0; i < size / 2; ++i) {
    *(c++) = inw(DISK_DATA(disk));
  }
}

/**
 * @brief 向磁盘disk中写size大小的数据到buf中
 *
 * @param disk
 * @param buf
 * @param size
 */
static void disk_write_data(disk_t *disk, void *buf, int size) {
  uint16_t *c = (uint16_t *)buf;

  for (int i = 0; i < size / 2; ++i) {
    outw(DISK_DATA(disk), *(c++));
  }
}

/**
 * @brief 等待磁盘disk准备好数据
 * 
 * @param disk 
 * @return int 
 */
static int disk_wait_data(disk_t *disk) {
  uint8_t status = 0;
  do {

    //轮询状态寄存器，若空闲或发生错误则进行下一步操作
    status = inb(DISK_STATUS(disk));
    if ((status & ( DISK_STATUS_BUSY 
                    | DISK_STATUS_DRQ 
                    | DISK_STATUS_ERR)) 
                    != DISK_STATUS_BUSY) {

        break;
    }
  } while (1);


  return (status & DISK_STATUS_ERR) ? -1 : 0;
  
}

/**
 * @brief 检测磁盘
 * 
 * @param disk 
 * @return int 
 */
static int identify_disk(disk_t *disk) {
    //将扇区数量和起始扇区即LBA lo,mid,hi 寄存器都设为0，并发送检测指令
    disk_send_cmd(disk, 0, 0, DISK_CMD_IDENTIFY);

    //读取状态寄存器判断是否出错，不出错则存在磁盘
    int err = inb(DISK_STATUS(disk));
    if (err == 0) {
        log_printf("%s doesn't exist", disk->name);
        return -1;
    }

    //等待磁盘数据就绪
    err = disk_wait_data(disk);
    if (err < 0) {
        log_printf("disk[%s]: read failed", disk->name);
        return -1;
    }

    //读取就绪的一个扇区的数据, 一共256个16位数据，其中包含了该磁盘的信息
    uint16_t buf[256];
    disk_read_data(disk, buf, sizeof(buf));

    //第100 到 103个数据，一共64位，保存了该磁盘的扇区总数量
    disk->sector_count = *(uint32_t *)(buf + 100);
    disk->sector_size = SECTOR_SIZE;

    return 0;
}

/**
 * @brief 打印磁盘相关信息
 * 
 * @param disk 
 */
static void print_disk_info(disk_t *disk) {
    log_printf("%s\n", disk->name);
    log_printf("\tport base: %x\n", disk->port_base);
    log_printf("\ttotal size: %d m\n", disk->sector_count * disk->sector_size / (1024*1024));
}

/**
 * @brief 初始化系统磁盘信息
 *
 */
void disk_init(void) {
  log_printf("Check disk...\n");

  kernel_memset(disk_table, 0, sizeof(disk_table));
  // 遍历并初始化化primary信道上的磁盘信息
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