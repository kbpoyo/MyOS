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

#include "cpu/idt.h"
/**
 * @brief  初始化互斥锁
 *
 * @param mutex
 */
void mutex_init(mutex_t *mutex) {
  ASSERT(mutex != (mutex_t *)0);

  mutex->locked_count = 0;
  mutex->owner = (task_t *)0;
  list_init(&mutex->wait_list);
}

/**
 * @brief  加锁
 *
 * @param mutex
 */
void mutex_lock(mutex_t *mutex) {
  idt_state_t state = idt_enter_protection();  // TODO:加锁

  // 1. 获取当前任务
  task_t *curr = task_current();
  if (curr == 0) {  //内核单进程模式，不需要互斥
    idt_leave_protection(state);  // TODO:解锁
    return;
  }

  // 2.判断该锁是否已被加锁
  if (mutex->locked_count == 0) {  
    //3.还未被加锁，则加锁并记录拥有该锁的任务
    mutex->locked_count++;
    mutex->owner = curr;
  } else if (mutex->owner == curr) {
    //4.已被加锁，但当前加锁请求的任务为当前锁的拥有者，直接再加锁即可
    mutex->locked_count++;
  } else {  
    //5.已被加锁，且当前任务不是锁的拥有者，则当前任务进入锁的等待队列，被阻塞住
    task_set_unready(curr);
    list_insert_last(&mutex->wait_list, &curr->wait_node);
    task_switch();
  }

  idt_leave_protection(state);  // TODO:解锁
}

/**
 * @brief  解锁
 *
 * @param mutex
 */
void mutex_unlock(mutex_t *mutex) {
  idt_state_t state = idt_enter_protection();  // TODO:加锁

  task_t *curr = task_current();
  if (curr == 0) {  //内核单进程模式，不需要互斥
    idt_leave_protection(state);  // TODO:解锁
    return;
  }

  // 1.判断当前任务是否是锁的拥有者
  if (mutex->owner == curr) {
    //2.当前任务是锁的拥有者,对锁进行一次解锁，并判断是否已完全解锁
    if (--mutex->locked_count == 0) {
      //3.锁已被完全解锁,将锁的所有者置空
      mutex->owner = (task_t*)0;
      //4.判断当前等待队列是否为空
      if (!list_is_empty(&mutex->wait_list)) { 
        //5.当前等待队列不为空,对锁进行加锁，并交给等待队列的第一个任务
        list_node_t *node = list_remove_first(&mutex->wait_list);
        task_t *task_wait = list_node_parent(node, task_t, wait_node);
        mutex->locked_count = 1;
        mutex->owner = task_wait;
        //6.让该任务进入就绪队列
        task_set_ready(task_wait);
      }
    }
  }

  idt_leave_protection(state);  // TODO:解锁
}
