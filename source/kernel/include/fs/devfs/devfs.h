/**
 * @file devfs.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 设备文件系统
 * @version 0.1
 * @date 2023-08-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DEVFS_H
#define DEVFS_H

#include "fs/fs.h"

//定义设备文件系统所管理的设备类型
typedef struct _devfs_type_t {
    const char *name;
    int dev_type;
    int file_type;
}devfs_type_t;


#endif
