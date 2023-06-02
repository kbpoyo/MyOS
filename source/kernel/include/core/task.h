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

//定义空闲进程的栈空间大小
#define EMPTY_TASK_STACK_SIZE 128

//设置任务进程的特权级标志位
#define TASK_FLAGS_SYSTEM   (1 << 0)  //内核特权级即最高特权级
#define TASK_FLAGS_USER   (0 << 0)  //用户特权级
// 定义任务状态枚举类型
typedef enum _task_state_t {
  TASK_CREATED,   // 已创建，任务被创建，但为加入就绪队列
  TASK_RUNNING,   // 运行态，正在运行
  TASK_SLEEP,     // 延时态，等待其线程唤醒或超时唤醒
  TASK_READY,     // 就绪态，可运行
  TASK_WAITTING,  // 等待态，只能等待其他线程唤醒
  TASK_BLOCKED,   // 阻塞态，等待外部资源或锁准备好
} state_t;

// 定义可执行任务的数据结构,即PCB进程控制块，书p406
typedef struct _task_t {
  state_t state;            //任务状态
  struct _task_t *parent;   //父进程控制块的地址
  int pid;                  //进程id
  uint32_t heap_start;      //进程堆空间的起始地址
  uint32_t heap_end;        //进程堆空间的结束地址
  int slice_max;            //任务所能拥有的最大时间分片数
  int slice_curr;           //任务当前的所拥有的时间分片数
  int sleep;                //当前任务延时的时间片数
  char name[TASK_NAME_SIZE];//任务名称
  list_node_t ready_node;   // 用于插入就绪队列的节点，标记task在就绪队列中的位置
  list_node_t task_node;    // 用于插入任务队列的节点，标记task在任务队列中的位置
  list_node_t wait_node;   //用于插入信号量对象的等待队列的节点，标记task正在等待信号量
  tss_t tss;                // 任务对应的TSS描述符
  uint32_t tss_selector;    // 任务对应的TSS选择子
} task_t;

int task_init(task_t *task, const char *name, uint32_t entry, uint32_t esp, uint32_t flag);
void task_switch_from_to(task_t *from, task_t *to);


// 定义任务管理器
typedef struct _task_manager_t {
  task_t *curr_task;  // 当前正在执行的任务

  list_t ready_list;  // 就绪队列，包含所有已准备好的可执行任务
  list_t task_list;  // 任务队列，包含所有的任务
  list_t sleep_list;  //延时队列，包含当前需要延时的任务

  task_t first_task;  // 执行的第一个任务
  task_t empty_task;  //一个空的空闲进程，当所有进程都延时运行时，让cpu运行空闲进程

  uint32_t app_code_selector; //应用程序代码段的选择子
  uint32_t app_data_selector; //应用程序数据段的选择子

} task_manager_t;

//定义任务入口参数的数据结构
typedef struct _task_args_t {
  uint32_t ret_addr;  //模拟调用函数压入的返回地址
  uint32_t argc;      //入口参数个数
  char * const *argv; //入口参数的字符串数组
}task_args_t;




void task_manager_init(void);
void task_first_init(void);
task_t *task_first_task(void);
void task_set_ready(task_t *task);
void task_set_unready(task_t *task);
void task_set_sleep(task_t *task, uint32_t slice);
void task_set_wakeup(task_t *task);
void task_slice_end(void);
void task_switch(void);
task_t* task_current(void);

//系统调用函数

void sys_sleep(uint32_t ms);
int sys_yield(void);
int sys_getpid(void);
int sys_fork(void);
int sys_execve(char *name, char * const *argv, char * const *env );
#endif