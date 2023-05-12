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
    int count = 3;
    int pid = fork();

    for (;;) {
        print_msg("pid=%d", pid);
        if (pid == 0) {
            print_msg("child pid=%d", getpid());
            print_msg("count=%d", count++);
            char const *argv[] = {"arg0", "arg1", "arg2", "arg3"};
            execve("/shell.elf", argv, 0);
        } else {
            print_msg("parent pid=%d", getpid());
            print_msg("count=%d", count);
            count += 2;

        }
        //msleep(100);
    }
    
    // int pid;
    // int i;
    // for (i = 0; i < 1024; ++i) {
    //     pid = fork();
    //     if (pid == 0)
    //         break;
    // }

    // print_msg("i = %d", i);
    // print_msg("pid = %d", getpid());

    // for (;;)
    // {
    // }
    

    return 0;
}