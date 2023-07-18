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


/**
 * @brief 根据命令程序名称查找并返回对应的命令结构
 * 
 * @param name 
 * @return const cli_cmd_t* 
 */
static const cli_cmd_t *find_builtin(const char *name) {
    for (const cli_cmd_t *cmd = cli.cmd_start; cmd < cli.cmd_end; ++cmd) {
        if (strcmp(cmd->name, name) != 0) {
            continue;
        }
        return cmd;
    }

    return (const cli_cmd_t*)0;
}

/**
 * @brief 根据命令结构和参数执行命令程序
 * 
 * @param cmd 
 * @param argc 
 * @param argv 
 */
static void run_builtin(const cli_cmd_t *cmd, int argc, const char **argv) {
    int ret = cmd->do_func(argc, argv);
    if (ret < 0) {
        fprintf(stderr, "error: %d\n", ret);
    }
}

int main(int argc, char** argv) {

    //1.打开shell对应的tty设备绑定为stdin
    open(argv[0], 0);
    //2.复用该0号描述符的文件，打开shell的stdout和stderr
    dup(0);
    dup(0);

    //3.初始化终端结构
    cli_init();



    //4.循环读取命令行输入并执行指令
    for (;;) {
        show_prompt();
        //4.1获取命令行输入并清空末尾的"\r\n"
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

        //4.2解析命令行输入的程序和参数
        int argc = 0;
        const char *argv[CLI_MAX_ARG_COUNT];
        memset(argv, 0, sizeof(argv));
        const char *split = " ";
        char *token = strtok(cli.curr_input, split);    //将字符串中的第一个split字符换成'\0'并放回起始索引
        while (token) {
            puts(token);
            argv[argc++] = token;
            token = strtok(NULL, split);    //填空默认从之前找到的位置的下一个位置开始
        }

        if (argc == 0) {
            continue;
        }

        //4.3 获取可执行的命令结构，并执行
        const cli_cmd_t *cmd = find_builtin(argv[0]);
        if (cmd) {
            run_builtin(cmd, argc, argv);
            continue;
        }
        
    }
    
}