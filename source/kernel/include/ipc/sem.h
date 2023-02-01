/**
 * @file sem.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义信号量
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SEM_H
#define SEM_H

#include "tools/list.h"

typedef struct  _sem_t {
    int count;
    list_t wait_list;
}sem_t;

void sem_init(sem_t *sem, int init_count);
void sem_wait(sem_t *sem);
void sem_notify(sem_t *sem);
int sem_count(sem_t *sem);

#endif