/**
 * @file mutex.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义互斥锁(互斥信号量)
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ipc/mutex.h"

/**
 * @brief  初始化互斥锁
 * 
 * @param mutex 
 */
void mutex_init(mutex_t *mutex) {
    ASSERT(mutex != (mutex_t*)0);

    mutex->locked_count = 0;
    mutex->owner = (task_t*)0;
    list_init(&mutex->wait_list);
}