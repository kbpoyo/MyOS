/**
 * @file fatfs.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief fat16文件系统
 * @version 0.1
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "fs/fatfs/fatfs.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "dev/dev.h"
#include "tools/log.h"
#include "core/memory.h"
#include "tools/klib.h"
#include <sys/fcntl.h>

/**
 * @brief 挂载fat文件系统
 * 
 * @param fs 
 * @param major 
 * @param minor 
 * @return int 
 */
int fatfs_mount(struct _fs_t *fs, int major, int minor) {

    //打开对应设备 即对应磁盘的对应分区
    int dev_id = dev_open(major, minor, (void *)0);
    if (dev_id < 0) {
        log_printf("open disk failed, major: %x, minor: %x\n", major, minor);
        goto mount_failed;
    }

    //分配一页来作为dbr区域的缓冲区
    dbr_t *dbr = (dbr_t *)memory_alloc_page();
    if (!dbr) {
        log_printf("mount failed: can't alloc buf\n");
        goto mount_failed;
    }


    //读取该分区的0号扇区，获取dbr的信息
    int cnt = dev_read(dev_id, 0, (char *)dbr, 1);
    if (cnt < 1) {
        log_printf("read dbr failed\n");
        goto mount_failed;
    }

    //解析fat表的信息到文件系统中
    fat_t *fat = &fs->fat_data;
    fat->bytes_per_sector = dbr->BPB_BytsPerSec;
    //因为保留区dbr从0扇区开始，且fat表紧邻dbr区
    //所以fat表的起始扇区，也就是保留区dbr的扇区总数
    fat->tbl_start_sector = dbr->BPB_RsvdSecCnt;
    fat->tbl_sectors = dbr->BPB_FATSz16;
    fat->tbl_cnt = dbr->BPB_NumFATs;
    fat->sec_per_cluster = dbr->BPB_SecPerClus;
    fat->root_ent_cnt = dbr->BPB_RootEntCnt;
    fat->root_start_sector = fat->tbl_start_sector + fat->tbl_sectors * fat->tbl_cnt;
    fat->data_start_sector = fat->root_start_sector + fat->root_ent_cnt * 32 / dbr->BPB_BytsPerSec;
    fat->cluster_bytes_size = fat->sec_per_cluster * dbr->BPB_BytsPerSec;
    fat->dbr_buffer = (uint8_t*)dbr;
    fat->fs = fs;

    if (fat->tbl_cnt != 2) {    //fat表数量一般为2， 不为2则出错
        log_printf("%s: fat table error: major: %x, minor: %x\n", major, minor);
        goto mount_failed;
    }

    if (kernel_strncmp((char*)(dbr->BS_FilSysType), "FAT16", 5) != 0) { //不是FAT16文件系统，出错
        log_printf("not a fat filesystem: major: %x, minor: %x\n", major, minor);
        goto mount_failed;

    }

    fs->type = FS_FAT16;
    fs->data = &fs->fat_data;
    fs->dev_id = dev_id;

    return 0;

mount_failed:
    if (dbr) {
        memory_free_page((uint32_t)dbr);
    }

    dev_close(dev_id);

    return -1;

} 


/**
 * @brief 卸载fat文件系统
 * 
 * @param fs 
 */
void fatfs_unmount(struct _fs_t *fs) {
    fat_t * fat = (fat_t *)fs->data;
    dev_close(fs->dev_id);

    memory_free_page((uint32_t)fat->dbr_buffer);
}
int fatfs_open(struct _fs_t *fs, const char *path, file_t *file) {


    return 0;
}
int fatfs_read(char *buf, int size, file_t *file) {
    return 0;

}
int fatfs_write(char *buf, int size, file_t *file) {
    return 0;

}
void fatfs_close(file_t *file) {

}
int fatfs_seek(file_t *file, uint32_t offset, int dir) {
    return 0;

}
int fatfs_stat(file_t *file, struct stat *st) {
    return 0;

}



//将fat文件系统的操作函数抽象给顶层文件系统使用
//类似于多态处理
fs_op_t fatfs_op = {
    .mount = fatfs_mount,
    .unmount = fatfs_unmount,
    .open = fatfs_open,
    .read = fatfs_read,
    .write = fatfs_write,
    .close = fatfs_close,
    .seek = fatfs_seek,
    .stat = fatfs_stat,
};