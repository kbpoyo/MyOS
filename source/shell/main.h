/**
 * @file main.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief shell程序
 * @version 0.1
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MAIN_H
#define MAIN_H

//shell输入缓冲区的大小
#define CLI_INPUT_SIZE  1024

//定义shell终端一次性接收的参数数量
#define CLI_MAX_ARG_COUNT   10

//定义ESC序列生成宏
#define  ESC_CMD2(Pn, cmd)  "\x1b["#Pn#cmd  //'#'用来将数字解析为字符串
#define ESC_CLEAR_SCREEN    ESC_CMD2(2, J)  //清屏序列
#define ESC_MOVE_CURSOR(row, col)   "\x1b["#row";"#col"H"   //移动光标序列
#define ESC_COLOR_ERROR ESC_CMD2(31, m) //更换前景色为红色
#define ESC_COLOR_DEFAULT ESC_CMD2(39, m)//更换前景色为默认白色

//定义终端命令结构
typedef struct _cli_cmd_t {
    const char *name;   //命令名称
    const char *usage;  //命令用法
    int(*do_func)(int argc, const char **arg);    //命令实现函数
}cli_cmd_t;


//定义终端结构
typedef struct _cli_t {
    char curr_input[CLI_INPUT_SIZE];    //终端缓冲区
    const cli_cmd_t *cmd_start; //终端支持的命令表的起始位置
    const cli_cmd_t *cmd_end;   //命令表的结束位置
    const char *prompt;  //每行的起始固定输出的提示符
}cli_t;




#endif
