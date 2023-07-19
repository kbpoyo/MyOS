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

#include "main.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_syscall.h"

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
static int do_help(int argc, const char **argv) {
  const cli_cmd_t *start = cli.cmd_start;
  while (start < cli.cmd_end) {
    printf("%s\t--> %s\n", start->name, start->usage);
    start++;
  }

  return 0;
}

/**
 * @brief 清屏操作
 *
 * @param argc
 * @param argv
 * @return int
 */
static int do_clear(int argc, const char **argv) {
  printf("%s", ESC_CLEAR_SCREEN);
  printf("%s", ESC_MOVE_CURSOR(0, 0));
  return 0;
}

/**
 * @brief 回显命令行信息
 *
 * @param argc
 * @param argv
 * @return int
 */
static int do_echo(int argc, const char **argv) {
  if (argc ==
      1) {  // 没有指定任何参数只输入了 echo，则等待用户输入信息以进行回显
    char msg_buf[128];
    fgets(msg_buf, sizeof(msg_buf), stdin);
    msg_buf[sizeof(msg_buf) - 1] = '\0';
    puts(msg_buf);
    return 0;
  }

  optind = 1;  // getopt每次查找都从optind处开始
  int count = 1;
  int ch;
  // getopt函数解析参数列表，n:表示查找-n选项，且必须紧跟着参数，
  // h表示查找-h选项，且不需要跟参数
  while ((ch = getopt(argc, (char *const *)argv, "n:h")) != -1) {
    switch (ch) {
      case 'h':
        puts("help:");
        puts("\techo any message");
        puts("\tUsage: echo [-n count] message");
        return 0;
      case 'n':
        count = atoi(optarg);  // optarg为newlib库自己声明的
        break;
      case '?':  // 找到未指定选项
        if (optarg) {
          fprintf(stderr,
                  ESC_COLOR_ERROR "unknown option: -%s\n" ESC_COLOR_DEFAULT,
                  optarg);
        }
        return -1;
      default:
        break;
    }
  }

    if (count == 0) {
    fprintf(stderr, ESC_COLOR_ERROR
            "echo: argument required for option '-n'\n" ESC_COLOR_DEFAULT);
    return -1;
    }
  if (optind >
      argc - 1) {  // argc - 1
                   // 为message的索引，optind>argc-1表示此次调用未携带message
    fprintf(stderr, ESC_COLOR_ERROR "message is empty\n" ESC_COLOR_DEFAULT);
    return -1;
  }
  

  const char *msg = argv[optind];
  for (int i = 0; i < count; ++i) {
    puts(msg);
  }
  return 0;
}

// 终端命令表
static const cli_cmd_t cmd_list[] = {
    {
        .name = "help",
        .usage = "help\t--list supported command",
        .do_func = do_help,
    },
    {
        .name = "clear",
        .usage = "clear\t--clear screen",
        .do_func = do_clear,
    },
    {
        .name = "echo",
        .usage = "echo [-n count] msg\t--echo msg [count] times",
        .do_func = do_echo,
    },
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

  return (const cli_cmd_t *)0;
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
    fprintf(stderr, "%s error: %d\n", cmd->name, ret);
  }
}

int main(int argc, char **argv) {
  // 1.打开shell对应的tty设备绑定为stdin
  open(argv[0], 0);
  // 2.复用该0号描述符的文件，打开shell的stdout和stderr
  dup(0);
  dup(0);

  // 3.初始化终端结构
  cli_init();

  // 4.循环读取命令行输入并执行指令
  for (;;) {
    show_prompt();
    // 4.1获取命令行输入并清空末尾的"\r\n"
    char *str = fgets(cli.curr_input, CLI_INPUT_SIZE, stdin);
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

    // 4.2解析命令行输入的程序和参数
    int argc = 0;
    const char *argv[CLI_MAX_ARG_COUNT];
    memset(argv, 0, sizeof(argv));
    const char *split = " ";
    char *token =
        strtok(cli.curr_input,
               split);  // 将字符串中的第一个split字符换成'\0'并放回起始索引
    while (token) {
      argv[argc++] = token;
      token = strtok(NULL, split);  // 填空默认从之前找到的位置的下一个位置开始
    }

    if (argc == 0) {
      continue;
    }

    // 4.3 获取可执行的命令结构，并执行
    const cli_cmd_t *cmd = find_builtin(argv[0]);
    if (cmd) {
      run_builtin(cmd, argc, argv);
      continue;
    }

    fprintf(stderr, ESC_COLOR_ERROR "Unknown command: %s\n" ESC_COLOR_DEFAULT,
            cli.curr_input);
  }
}