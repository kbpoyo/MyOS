/**
 * @file devfs.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 设备文件系统
 * @version 0.1
 * @date 2023-08-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "fs/devfs/devfs.h"
#include "dev/dev.h"
#include "fs/file.h"

/**
 * @brief 挂载设备文件系统
 * 
 * @param fs 
 * @param major 
 * @param minor 
 * @return int 
 */
int devfs_mount(struct _fs_t *fs, int major, int minor) {
    fs->type = FS_DEVFS;
    return fs->type;
}

/**
 * @brief 卸载设备文件系统
 * 
 * @param fs 
 */
void devfs_unmount(struct _fs_t *fs) {
} 


/**
 * @brief 打开设备文件系统
 * 
 * @param fs 
 * @param path 
 * @param file 
 * @return int 
 */
int devfs_open(struct _fs_t *fs, const char *path, file_t *file) {
    return 0;
};

/**
 * @brief 读取设备文件系统
 * 
 * @param buf 
 * @param size 
 * @param file 
 * @return int 
 */
int devfs_read(const char *buf, int size, file_t *file) {
    return dev_read(file->dev_id, file->pos, buf, size);
}

/**
 * @brief 写入设备文件系统
 * 
 * @param buf 
 * @param size 
 * @param file 
 * @return int 
 */
int devfs_write(const char *buf, int size, file_t *file) {
    return dev_write(file->dev_id, file->pos, buf, size);
}

/**
 * @brief 关闭设备文件系统
 * 
 * @param file 
 */
void devfs_close(file_t *file) {
    dev_close(file->dev_id);
}

/**
 * @brief 使设备文件系统当前读取位置向dir方向偏移offset个字节
 * 
 * @param file 
 * @param offset 
 * @param dir 
 * @return int 
 */
int devfs_seek(file_t *file, uint32_t offset, int dir) {
    //设备文件系统不支持偏移调用，直接返回-1;
    return -1;
}


/**
 * @brief 获取设备文件系统的状态
 * 
 * @param file 
 * @param st 
 * @return int 
 */
int devfs_stat(file_t *file, struct stat *st) {
    //设备文件系统不支持该调用
    return -1;
}

//将设备文件系统的操作函数抽象给顶层文件系统使用
//类似于多态处理
fs_op_t devfs_op = {
    .mount = devfs_mount,
    .unmount = devfs_unmount,
    .open = devfs_open,
    .read = devfs_read,
    .write = devfs_write,
    .close = devfs_close,
    .seek = devfs_seek,
    .stat = devfs_stat,
};