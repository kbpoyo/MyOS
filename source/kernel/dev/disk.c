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
#include "dev/dev.h"
#include "cpu/idt.h"
#include "common/exc_frame.h"

// 系统磁盘表
static disk_t disk_table[DISK_CNT];
//磁盘锁
static mutex_t mutex;
//磁盘操作信号量
static sem_t op_sem;
//标志位，置1表示用户进程在对磁盘进行操作
//因为在loader程序在加载内核时触发过磁盘的中断
//磁盘中断请求一直存在，当磁盘中断处理程序注册后
//cpu就会处理该中断，使op_sem的信号量进行无效+1
//所以需要标志位来区分是用户程序触发的该中断
static uint8_t task_on_op = 0;

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
 * @brief 从buf中向磁盘disk写size大小的数据
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
 * @brief 检测磁盘disk数据是否准备好
 *        并检测磁盘是否发生错误
 * 
 * @param disk 
 * @return int 
 */
static int disk_wait_data(disk_t *disk) {
  uint8_t status = 0;
  do {

    //轮询状态寄存器
    //若磁盘空闲则进行下一步操作
    //若磁盘忙碌 但有数据就绪又或者有错误发生，则进行下一步操作
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
 * @brief 检测磁盘disk的分区表信息
 * 
 * @param disk 
 * @return int 
 */
static int detect_part_info(disk_t *disk) {
  mbr_t mbr;
  //1.发送读取指令
  disk_send_cmd(disk, 0, 1, DISK_CMD_READ);
  //2.等待磁盘数据就绪
  int err = disk_wait_data(disk);
  if (err < 0) {
    log_printf("read mbr failed!\n");
    return err;
  }

  //3.读取0扇区的mbr,并将由bios检测并填充的分区表信息
  //读取到disk的partinfo结构中
  disk_read_data(disk, &mbr, sizeof(mbr));
  part_item_t *item = mbr.part_item;
  partinfo_t *part_info = disk->partinfo + 1;
  for (int i = 0; i < MBR_PRIMARY_PART_NR; ++i, ++item, ++part_info) {
    part_info->type = item->system_id;
    if (part_info->type == FS_INVALID) {  //无效分区，不使用
      part_info->total_sectors = 0;
      part_info->start_sector = 0;
      part_info->disk = (disk_t *)0;
    } else {  //分区有效，记录分区信息
      kernel_sprintf(part_info->name, "%s%d", disk->name, i);
      part_info->disk = disk;
      part_info->start_sector = item->relative_sector;
      part_info->total_sectors = item->total_sectors;
    }
  }
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

    //初始化磁盘分区信息
    //用partinfo将整个磁盘视为一个大分区
    partinfo_t *part_info = disk->partinfo + 0;
    part_info->disk = disk;
    kernel_sprintf(part_info->name, "%s%d", disk->name, 0);
    part_info->start_sector = 0;
    part_info->total_sectors = disk->sector_count;
    part_info->type = FS_INVALID;

    //读取并检测磁盘的分区表信息
    detect_part_info(disk);

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

    for (int i = 0; i < DISK_PRIMARY_PART_CNT; ++i) {
      partinfo_t *part_info = disk->partinfo + i;
      if (part_info->type != FS_INVALID) {
        log_printf("\t%s: type: %x, start sector: %d, sector count: %d\n",
            part_info->name, part_info->type, part_info->start_sector, 
            part_info->total_sectors);
      }
    }
}

/**
 * @brief 初始化系统磁盘信息
 *
 */
void disk_init(void) {
  log_printf("Check disk...\n");

  kernel_memset(disk_table, 0, sizeof(disk_table));

  //初始化磁盘锁与操作信号量
  mutex_init(&mutex);
  sem_init(&op_sem, 0);

  // 遍历并初始化化primary信道上的磁盘信息
  for (int i = 0; i < DISK_PER_CHANNEL; ++i) {
    disk_t *disk = disk_table + i;
    kernel_sprintf(disk->name, "sd%c", i + 'a');
    disk->drive = i == 0 ? DISK_MASTER : DISK_SLAVE;
    disk->port_base = IOBASE_PRIMARY;
    disk->mutex = &mutex;
    disk->op_sem = &op_sem;

    int err = identify_disk(disk);
    if (err == 0) {
      print_disk_info(disk);
    }
  }
}


/**
 * @brief 打开磁盘设备
 * 
 * @param dev 
 * @return int 
 */
int disk_open (device_t *dev) {
  //对磁盘的编号为 a , b
  //对扇区的编号为0, 1, 2, 3, 4, 0分区包含整个磁盘
  //设备索引编号0xa0表示 a磁盘上的0分区

  //获取磁盘在系统磁盘表中的索引
  int disk_index = (dev->dev_index >> 4) - 0xa;   
  //获取分区的索引
  int part_index = dev->dev_index & 0xf;

  if (disk_index >= DISK_CNT || part_index >= DISK_PRIMARY_PART_CNT) {
      log_printf("device index error: %d\n", dev->dev_index);
      return -1;
  }

  //获取磁盘对象
  disk_t *disk = disk_table + disk_index;
  if (disk->sector_count == 0) {
    log_printf("disk not exist, device: sd%d", dev->dev_index);
    return -1;
  }

  //获取分区对象
  partinfo_t *part_info = disk->partinfo + part_index;
  if (part_info->total_sectors == 0) {
    log_printf("part not exist\n");
    return -1;
  }

  //记录分区信息
  dev->data = (void *)part_info;

  //注册中断处理程序,并开启中断
  idt_install(IRQ14_HARDDISK_PRIMARY, (idt_handler_t)exception_handler_primary_disk);
  idt_enable(IRQ14_HARDDISK_PRIMARY);

  return 0;
}
/**
 * @brief 读磁盘
 * 
 * @param dev 设备对象，记录了磁盘分区信息
 * @param addr 读取的起始扇区相对于dev指定分区的偏移量
 * @param buf 读取缓冲区
 * @param size 读取扇区数
 * @return * int 
 */
int disk_read(device_t *dev, int addr, char *buf, int size) {

  //获取要读取的分区信息
  partinfo_t *part_info = (partinfo_t*)dev->data;
  if (!part_info) {
    log_printf("Get part info failed. devce: %d\n", dev->dev_index);
    return -1;
  }

  //获取磁盘对象
  disk_t *disk = part_info->disk;
  if (disk == (disk_t *)0) {
    log_printf("No disk, device: %d\n", dev->dev_index);
    return -1;
  }

  //TODO:加锁
  mutex_lock(disk->mutex);  //确保磁盘io操作的原子性
  if (task_current()) {
    task_on_op = 1; //将标志位置1，表示内核在执行磁盘操作
  }

  //发送读取指令
  disk_send_cmd(disk, part_info->start_sector + addr, size, DISK_CMD_READ);

  //
  int cnt;
  for (cnt = 0; cnt < size; ++cnt, buf += disk->sector_size) {
    //等待磁盘的中断，代表准备就绪
    sem_wait(disk->op_sem);

    //因为信号量已经让进程等待了，所以此处只是检测是否发生错误
    int err = disk_wait_data(disk);
    if (err < 0) {
      log_printf("disk[%s] read error: start sector %d, count: %d",
          disk->name, addr, size);
          cnt = -1;
          break;
    }

    //磁盘每次读取都是按一个扇区的大小进行读取
    disk_read_data(disk, buf, disk->sector_size);
  }

  //TODO:解锁
  mutex_unlock(disk->mutex);

  return cnt;
}

/**
 * @brief 写磁盘
 * 
 * @param dev 
 * @param addr 
 * @param buf 
 * @param size 
 * @return int 
 */
int disk_write(device_t *dev, int addr, char *buf, int size) {

   //获取要读取的分区信息
  partinfo_t *part_info = (partinfo_t*)dev->data;
  if (!part_info) {
    log_printf("Get part info failed. devce: %d\n", dev->dev_index);
    return -1;
  }

  //获取磁盘对象
  disk_t *disk = part_info->disk;
  if (disk == (disk_t *)0) {
    log_printf("No disk, device: %d\n", dev->dev_index);
    return -1;
  }

  //TODO:加锁
  mutex_lock(disk->mutex);  //确保磁盘io操作的原子性
  if (task_current()) {
    task_on_op = 1; //将标志位置1，表示用户进程在执行磁盘操作
  }

  //发送读取指令
  disk_send_cmd(disk, part_info->start_sector + addr, size, DISK_CMD_WRITE);

  //
  int cnt;
  for (cnt = 0; cnt < size; ++cnt, buf += disk->sector_size) {
    //磁盘每次写入都是按一个扇区的大小进行写入
    disk_write_data(disk, buf, disk->sector_size);
    //等待磁盘的中断，代表写入完成
    sem_wait(disk->op_sem);
    

    //检测是否发生错误
    int err = disk_wait_data(disk);
    if (err < 0) {
      log_printf("disk[%s] read error: start sector %d, count: %d",
          disk->name, addr, size);
          cnt = -1;
          break;
    } 
  }

  //TODO:解锁
  mutex_unlock(disk->mutex);

  return cnt;
}

/**
 * @brief 向磁盘发送控制指令
 * 
 * @param dev 
 * @param cmd 
 * @param arg0 
 * @param arg1 
 * @return int 
 */
int disk_control(device_t *dev, int cmd, int arg0, int arg1) {

  return -1;
}


/**
 * @brief 磁盘的中断处理函数
 *        磁盘执行完读操作或写操作后会触发中断
 * 
 * @param frame 
 */
void do_handler_primary_disk(exception_frame_t *frame) {
  //中断抢占成功，发送eoi信号，清除中断请求
  pic_send_eoi(IRQ14_HARDDISK_PRIMARY);

  //当用户程序触发磁盘中断时，唤醒等待进程
  if (task_on_op) {
    //磁盘数据准备就绪或磁盘写入完成，唤醒等待进程
    sem_notify(&op_sem);
  }
}


/**
 * @brief 关闭磁盘
 * 
 * @param dev 
 */
void disk_close(device_t *dev) {

}


//操作disk结构的函数表
dev_desc_t dev_disk_desc = {
    .dev_name = "disk",
    .dev_type = DEV_DISK,
    .open = disk_open,
    .read = disk_read,
    .write = disk_write,
    .control = disk_control,
    .close = disk_close
};