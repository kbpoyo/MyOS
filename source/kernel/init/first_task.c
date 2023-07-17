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
#include "dev/tty.h"

int first_main(void) {
    // int count = 3;
    // int pid = fork();

    // for (;;) {
    //     // print_msg("pid=%d", pid);
    //     if (pid == 0) {
    //         // print_msg("child pid=%d", getpid());
    //         // print_msg("count=%d", count++);
    //         char const *argv[] = {"arg0", "arg1", "arg2", "arg3"};
    //         execve("/shell.elf", argv, 0);
    //     } else {
    //         // print_msg("parent pid=%d", getpid());
    //         // print_msg("count=%d", count);
    //         count += 2;

    //     }
    //     //msleep(100);
    // }
    
    // int pid;
    // int i;
    // for (i = 0; i < 1; ++i) {
    //     pid = fork();
    //     if (pid == 0)
    //         break;
    // }

    // char * const argv[] = {"arg0", "arg1", "arg2", "arg3", 0};
    // if (pid != 0) execve("/shell.elf", argv, 0);
    
    // for (;;)
    // {
    // }

    //为每个tty设备创建一个进程
    for (int i = 0; i < TTY_TABLE_SIZE; ++i) {
        int pid = fork();
        if (pid < 0) {
            print_msg("create shell failed.", 0);
            break;
        } else if (pid == 0) {
            char tty_num[5] = "tty:?";
            tty_num[4] = i + '0';
            char * const argv[] = {tty_num, 0};
            execve("/sehll.elf", argv, 0);
            while (1)   {
                msleep(1000);
            }
            
        }
    }    

    for (;;) {
        msleep(1000);
    }

    return 0;
}