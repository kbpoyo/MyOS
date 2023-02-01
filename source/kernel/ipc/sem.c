/**
 * @file sem.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义信号量
 * @version 0.1
 * @date 2023-02-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ipc/sem.h"
#include "core/task.h"
#include "cpu/idt.h"


/**
 * @brief  初始化信号量对象
 * 
 * @param sem 信号量对象
 * @param init_count 初始化信号量数量 
 */
void sem_init(sem_t *sem, int init_count) {
    ASSERT(sem != (sem_t*)0);
    sem->count = 0;
    list_init(&sem->wait_list);
}

/**
 * @brief  当前任务获取信号量，获取到则继续执行，
 *         未获取到则进入等待队列，不可继续执行，
 *         信号量相当于入场券，获取信号量之后才有资格访问信号量保护的资源
 * 
 * @param sem 
 * @param init_count 
 */
void sem_wait(sem_t *sem) {
    
    idt_state_t state = idt_enter_protection();//TODO:加锁

    //1.判断信号量是否还有剩余
    if(sem->count > 0) {//有剩余，直接使用，任务获取信号量继续执行
        --sem->count;   
    } else {//没有剩余，任务进入延时队列等待信号量
        task_t *curr = task_current();
        //2.将当前任务从就绪队列中取下
        task_set_unready(curr); 
        //3.将当前任务加入到信号量等待队列
        list_insert_last(&sem->wait_list, &curr->wait_node); 
        //4.切换任务
        task_switch();
    }

    idt_leave_protection(state);//TODO:解锁
}

/**
 * @brief 任务将信号量归还，即归还入场券，让给等待队列中的任务
 *        等待队列中若有任务则直接获取该信号量，继续执行即访问资源
 *         若没有任务，则信号量对象记录的信号量数量加1，预留给下一次获取
 * 
 * @param sem 
 */
void sem_notify(sem_t *sem) {
    idt_state_t state = idt_enter_protection();//TODO:加锁
    
    if (!list_is_empty(&sem->wait_list)) {
        list_node_t *node = list_remove_first(&sem->wait_list);
        task_t *task = list_node_parent(node, task_t, wait_node);
        task_set_ready(task);
    } else {
        sem->count++;
    }

    idt_leave_protection(state);//TODO:解锁

}

int sem_count(sem_t *sem) {
    idt_state_t state = idt_enter_protection();//TODO:加锁
    int count = sem->count;
    idt_leave_protection(state);//TODO:解锁

    return count;
}