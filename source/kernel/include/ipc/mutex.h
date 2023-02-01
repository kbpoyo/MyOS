/**
 * @file mutex.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义互斥锁（互斥信号量)
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MUTEX_H
#define MUTEX_H

#include "tools/list.h"
#include "core/task.h"

typedef struct  _mutex_t{
    task_t *owner;      //当前锁的拥有者
    int locked_count;   //当前锁被上锁了几次
    list_t wait_list;   //等待该锁的任务队列
}mutex_t;


void mutex_init(mutex_t *mutex);


#endif