/**
 * @file task.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  
 * @version 0.1
 * @date 2023-01-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TASK_H
#define TASK_H
#include "cpu/tss.h"
#include "common/types.h"

//定义可执行任务的数据结构,即PCB控制块，书p406
typedef struct _task_t {
    uint32_t stack_esp;    //保存任务所使用的栈空间的栈顶指针esp的值
}task_t;

int task_init(task_t *task, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t *from, task_t *to);

#endif