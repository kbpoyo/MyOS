/**
 * @file file.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 文件描述结构
 * @version 0.1
 * @date 2023-07-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef FILE_H
#define FILE_H

#include "common/types.h"

#define FILE_TABLE_SIZE 2048
#define FILE_NAME_SIZE  32

//文件类型的枚举
typedef enum _file_type_t {
    FILE_UNKNOWN = 0,
    FILE_TTY,
    FILE_DIR,
    FILE_NORMAL,

}file_type_t;


struct _fs_t;

typedef struct  _file_t {
    //通用
    char file_name[FILE_NAME_SIZE]; //文件名
    file_type_t type;   //文件类型
    int ref;            //文件打开次数
    struct _fs_t *fs;   //文件所属文件系统
   
    //供设备文件系统使用
    int dev_id;     //文件对应的设备id

    //供fat文件系统使用
    int pos;        //记录当前文件读取的位置
    int mode;       //文件的读写模式
    uint32_t size;  //文件大小
    int sblk;       //文件起始簇号或块号
    int cblk;       //文件当前读取的簇号或块号
    int p_index;    //文件所属目录项在根目录区的索引
   
}file_t;


file_t *file_alloc(void);
void file_free(file_t *file);
void file_table_init(void);
void file_inc_ref(file_t *file);

#endif