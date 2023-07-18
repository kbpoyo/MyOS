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

#include <stdio.h>
#include <string.h>
#include "lib_syscall.h"
#include "main.h"


static char cmd_buf[512];
static const char *prompt = "sh >>";
static cli_t cli;


/**
 * @brief 命令行help函数,打印所有命令名称以及用法
 * 
 * @param argc 
 * @param arg 
 * @return int 
 */
static int do_help(int argc, char **arg) {
    const cli_cmd_t *start = cli.cmd_start;
    while (start < cli.cmd_end) {
        printf("%s --> %s\n", start->name, start->usage);
        start++;
    }
    
    
    return  0;   
}

//终端命令表
static const cli_cmd_t cmd_list[] = {
    {
        .name = "help",
        .usage = "help --list supported command",
        .do_func = do_help,   
    }
};

/**
 * @brief 初始化终端结构
 * 
 */
static void cli_init() {
    cli.prompt = prompt;
    memset(cli.curr_input, 0, CLI_INPUT_SIZE);
    cli.cmd_start = cmd_list;
    cli.cmd_end = cmd_list + sizeof(cmd_list) / sizeof(cmd_list[0]);
}

/**
 * @brief 打印终端每行固定的提示符
 * 
 */
static void show_prompt(void) {
    printf("%s", cli.prompt);
    fflush(stdout);
}

int main(int argc, char** argv) {

    open(argv[0], 0);
    dup(0);
    dup(0);

    cli_init();


    // printf("hello from shell\n");

    for (;;) {
        //printf("shell pid=%d\n", getpid());
        //msleep(1000);
        show_prompt();
        char * str = fgets(cli.curr_input, CLI_INPUT_SIZE, stdin);
        if (!str) {
            continue;
        }
        char *cr = strchr(cli.curr_input, '\n');
        if (cr) {
            *cr = '\0'; 
        }
        cr = strchr(cli.curr_input, '\r');
        if (cr) {
            *cr = '\0';
        }

        puts(str);
        puts(cli.curr_input);
    }
    
}