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


static char cmd_buf[512];

int main(int argc, char** argv) {
#ifdef SWITCH
    sbrk(0);
    sbrk(100);
    sbrk(200);
    sbrk(4096*2 + 200);
    sbrk(4096*5 + 1234);

    printf("\0337Hello,word!\0338123\n");  // ESC 7,8 输出123lo,word!
    printf("\033[31;42mHello,word!\033[39;49m123\n");  // ESC [pn m, Hello,world红色，>其余绿色
    printf("123\033[2DHello,word!\n");  // 光标左移2，1Hello,word!
    printf("123\033[2CHello,word!\n");  // 光标右移2，123  Hello,word!

    printf("\033[31m");  // ESC [pn m, Hello,world红色，其余绿色
    printf("\033[10;10H test!\n");  // 定位到10, 10，test!
    printf("\033[20;20H test!\n");  // 定位到20, 20，test!
    printf("\033[32;25;39m123\n");  // 

    printf("\033[2J");//清空屏幕

    printf("hello from shell\n");
    printf("os version: %s\n", "1.0.0");
    printf("%05d, %-5d, %d\n", 1, 2, 3);

    printf("main pid %d\n", getpid());
    for (int i = 0; i < argc; ++i) {
        printf("arg: %s\n", argv[i]);
    }

    int pid = fork();
    yield();
    if (pid > 0) printf("parent pid=%d\n", pid);
    else printf("chiled pid=%d\n", pid);

#endif

    open("tty:0", 0);
    dup(0);
    dup(0);

    fprintf(stderr, "error\n");
    

    // printf("hello from shell\n");

    for (;;) {
        //printf("shell pid=%d\n", getpid());
        //msleep(1000);
        gets(cmd_buf);
        puts(cmd_buf);
    }
    
}