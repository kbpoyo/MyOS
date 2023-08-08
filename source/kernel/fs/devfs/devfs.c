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
#include "tools/klib.h"
#include "tools/log.h"

//定义设备文件系统管理的类型表
static devfs_type_t devfs_type_list[] = {
    {
        .name = "tty",
        .dev_type = DEV_TTY,
        .file_type = FILE_TTY,
    },  //tty设备类型
};


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
    //遍历设备类型表，获取需要打开的设备的信息
    for (int i = 0; i < sizeof(devfs_type_list) / sizeof(devfs_type_list[0]); ++i) {
        devfs_type_t *type = devfs_type_list + i;

        int type_name_len = kernel_strlen(type->name);
        if (kernel_strncmp(path, type->name, type_name_len) == 0) {
            int minor;
            //判断路径是否正确并读取路径中的设备号
            if (kernel_strlen(path) > type_name_len && path_to_num(path + type_name_len, &minor) < 0) {
                log_printf("Get device num failed. %s", path);
                break;
            }

            //打开设备
            int dev_id = dev_open(type->dev_type, minor, (void*)0);
            if (dev_id < 0) {
                log_printf("open device failed: %s", path);
                break;
            }

            //打开成功，初始化file结构，用file记录文件信息
            file->dev_id = dev_id;
            file->pos = 0;
            file->size = 0;
            file->type = type->file_type;
            file->ref = 1;

            return 0;
        }
    }

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
int devfs_read(char *buf, int size, file_t *file) {
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
int devfs_write(char *buf, int size, file_t *file) {
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