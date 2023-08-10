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
#include <sys/fcntl.h>

int fatfs_mount(struct _fs_t *fs, int major, int minor) {

    return 0;

}   //挂载文件
void fatfs_unmount(struct _fs_t *fs) {

}
int fatfs_open(struct _fs_t *fs, const char *path, file_t *file) {

    return 0;
}
int fatfs_read(const char *buf, int size, file_t *file) {
    return 0;

}
int fatfs_write(const char *buf, int size, file_t *file) {
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