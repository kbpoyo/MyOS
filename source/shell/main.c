/**
 * @file main.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 外部测试程序，用于操作系统加载
 * @version 0.1
 * @date 2023-05-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "lib_syscall.h"

int main(int argc, char** argv) {
    print_msg("main pid %d", getpid());
    for (int i = 0; i < argc; ++i) {
        print_msg("arg: %s", argv[i]);
    }

    int pid = fork();
    yield();
    if (pid > 0) print_msg("parent pid=%d", pid);
    else print_msg("chiled pid=%d", pid);

    for (;;) {
        print_msg("shell pid=%d", getpid());
        msleep(1000);
    }
    
}