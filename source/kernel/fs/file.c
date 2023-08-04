/**
 * @file file.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 文件结构描述
 * @version 0.1
 * @date 2023-07-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "fs/file.h"
#include "ipc/mutex.h"
#include "tools/klib.h"

static file_t file_table[FILE_TABLE_SIZE];
static mutex_t file_alloc_mutex;    //互斥锁，保护file_table的正确分配


/**
 * @brief 初始化file_table
 * 
 */
void file_table_init(void) {
    mutex_init(&file_alloc_mutex);
    kernel_memset(file_table, 0, sizeof(file_table));
}

/**
 * @brief 从file_table中分配一个file结构
 * 
 * @return file_t* 
 */
file_t *file_alloc(void) {
    file_t *file = (file_t*)0;

    //TODO:加锁
    mutex_lock(&file_alloc_mutex);

    //在file_table中获取分配一个资源
    for (int i = 0; i < FILE_TABLE_SIZE; ++i) {
        file_t * p_file = file_table + i;
        if (p_file->ref == 0) { //当前资源未被分配
            kernel_memset(p_file, 0, sizeof(file_t));
            p_file->ref = 1;    //记录被外部引用
            file = p_file;
            break;
        }
    }

    //TODO:解锁
    mutex_unlock(&file_alloc_mutex);
    return file;
}


/**
 * @brief 释放一个文件结构资源
 * 
 * @param file 
 */
void file_free(file_t *file) {

    //TODO:加锁
    mutex_lock(&file_alloc_mutex);
    
    if (file->ref > 0) {    //引用计数减1
        file->ref--;
    }

    //TODO:解锁
    mutex_unlock(&file_alloc_mutex);
}

/**
 * @brief 增加文件file的引用计数
 * 
 * @param file 
 */
void file_inc_ref(file_t *file) {

    //TODO:加锁
    mutex_lock(&file_alloc_mutex);
    
    file->ref++;

    //TODO:解锁
    mutex_unlock(&file_alloc_mutex);
}