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
#include <stdio.h>

int main(int argc, char** argv) {
    sbrk(0);
    sbrk(100);
    sbrk(200);
    sbrk(4096*2 + 200);
    sbrk(4096*5 + 1234);

    printf("hello from shell\n");
    printf("os version: %s\n", "1.0.0");
    printf("%05d, %-5d, %d\n", 1, 2, 3);

    printf("main pid %d", getpid());
    for (int i = 0; i < argc; ++i) {
        printf("arg: %s\n", argv[i]);
    }

    int pid = fork();
    yield();
    if (pid > 0) printf("parent pid=%d\n", pid);
    else printf("chiled pid=%d\n", pid);

    for (;;) {
        printf("shell pid=%d\n", getpid());
        msleep(1000);
    }
    
}