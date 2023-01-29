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
#include "common/types.h"
#include "cpu/tss.h"
#include "tools/list.h"

// 定义任务名称缓冲区大小
#define TASK_NAME_SIZE 32

//定义每个进程所能拥有的时间切片数量
#define TASK_TIME_SLICE_DEFAULT 10

// 定义任务状态枚举类型
typedef enum _task_state_t {
  TASK_CREATED,   // 已创建，任务被创建，但为加入就绪队列
  TASK_RUNNING,   // 运行态，正在运行
  TASK_SLEEP,     // 延时态，等待其线程唤醒或超时唤醒
  TASK_READY,     // 就绪态，可运行
  TASK_WAITTING,  // 等待态，只能等待其他线程唤醒
  TASK_BLOCKED,   // 阻塞态，等待外部资源或锁准备好
} state_t;

// 定义可执行任务的数据结构,即PCB控制块，书p406
typedef struct _task_t {
  state_t state;            //任务状态
  int slice_max;            //任务所能拥有的最大时间分片数
  int slice_curr;           //任务当前的所拥有的时间分片数
  char name[TASK_NAME_SIZE];//任务名称
  list_node_t ready_node;   // 用于插入任务队列的节点，标记task在就绪队列中的位置
  list_node_t task_node;    // 用于插入任务队列的节点，标记task在任务队列中的位置
  tss_t tss;                // 任务对应的TSS描述符
  uint32_t tss_selector;    // 任务对应的TSS选择子
} task_t;

void task_init(task_t *task, const char *name, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t *from, task_t *to);


// 定义任务管理器
typedef struct _task_manager_t {
  task_t *curr_task;  // 当前正在执行的任务

  list_t ready_list;  // 就绪队列，包含所有已准备好的可执行任务
  list_t task_list;  // 任务队列，包含所有的任务

  task_t first_task;  // 执行的第一个任务
} task_manager_t;

void task_manager_init(void);
void task_first_init(void);
task_t *task_first_task(void);
void task_set_ready(task_t *task);
int sys_yield(void);
void task_slice_end(void);
#endif