/**
 * @file first_task.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 第一个任务进程，实现与操作系统的代码隔离
 * @version 0.1
 * @date 2023-04-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/log.h"
#include "core/task.h"
#include  "applib/lib_syscall.h"

int first_main(void) {
    int pid = getpid();
    for (;;) {
        //log_printf("first task.");
        msleep(1000);
    }
    
    return 0;
}