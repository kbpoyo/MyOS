/**
 * @file fs.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 文件操作
 * @version 0.1
 * @date 2023-05-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FS_H
#define FS_H

#include "fs/file.h"
#include "tools/list.h"
#include "ipc/mutex.h"
#include "fatfs/fatfs.h"

//类型声明，用以链接newlib库
struct stat;

struct _fs_t;

//定义对文件结构进行操作的函数表结构
typedef struct _fs_op_t {
    int (*mount)(struct _fs_t *fs, int major, int minor);   //挂载文件
    void (*unmount)(struct _fs_t *fs); //卸载文件
    int (*open)(struct _fs_t *fs, const char *path, file_t *file);
    int (*read)(const char *buf, int size, file_t *file);
    int (*write)(const char *buf, int size, file_t *file);
    void (*close)(file_t *file);
    int (*seek)(file_t *file, uint32_t offset, int dir);
    int (*stat)(file_t *file, struct stat *st);

}fs_op_t;


//定义挂载点名称的大小
#define FS_MOUNT_POINT_SIZE    512

//定义文件系统类型的枚举
typedef enum _fs_type_t {
    FS_DEVFS,  //设备文件系统
    FS_FAT16,   //fat文件系统
}fs_type_t;

//定义文件系统的顶层抽象类型
typedef struct _fs_t {
    //抽象文件系统类型对应的挂载的真正文件系统名称
    char mount_point[FS_MOUNT_POINT_SIZE];  
    fs_type_t type;
    fs_op_t *op;
    void *data; //调用设备或文件时传入的数据
    int dev_id; //设备id
    list_node_t node;
    mutex_t *mutex;

    union { //当是fat16文件系统时，存储fat表的数据
        fat_t fat_data;
    };
    
}fs_t;



void fs_init(void);

int path_to_num(const char * path, int *num);
const char *path_next_child(const char *path);

int sys_open(const char *name, int flags, ...);
int sys_read(int file, char *ptr, int len);
int sys_write(int file, char *ptr, int len);
int sys_lseek(int file, int offset, int pos);
int sys_close(int file);
int sys_isatty(int file);
int sys_fstat(int file, struct stat *st);
int sys_dup(int file);
#endif