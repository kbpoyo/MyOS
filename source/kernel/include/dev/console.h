/**
 * @file console.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 控制台设备
 * @version 0.1
 * @date 2023-06-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONSOLE_H
#define CONSOLE_H
#include "common/types.h"

#define CONSOLE_DISP_START_ADDR 0xb8000 //现存空间的起始地址
#define CONSOLE_DISP_END_ADDR   (0xb8000 + 32*1024)   //现存空间的结束地址
#define CONSOLE_ROW_MAX         25  //上电之后BIOS会初始化屏幕为25行80列
#define CONSOLE_CLO_MAX         80

//对printf的字符序列做处理所预定义的宏
#define ASCII_ESC               0x1b //\033

//封装显示的字符结构
typedef union  _disp_char_t {
    struct {
        char c; //显示的字符
        char foreground : 4;    //4位前景色
        char background : 3;    //3位背景色   
    };
    
    uint16_t v;  //第一个字节为字符，第二个字节为前景色与背景色  
}disp_char_t;

//定义颜色枚举
typedef enum {
    COLOR_Black = 0,
    COLOR_Blue,
    COLOR_Green,
    COLOR_Cyan,
    COLOR_Red,
    COLOR_Magenta,
    COLOR_Brown,
    COLOR_Gray,
    COLOR_DarkGray,
    COLOR_Light_Bule,
    COLOR_Light_Green,
    COLOR_Light_Cyan,
    COLOR_Light_Red,
    COLOR_Light_Magenta,
    COLOR_Yellow,
    COLOR_White,
}color_t;


//封装终端对象
typedef struct _console_t {
    enum {
        CONSOLE_WRITE_NORMAL,   //当前终端正在写入普通字符
        CONSOLE_WRITE_ESC,      //当前终端正在写入ESC字符
    }write_state;   
    disp_char_t *disp_base; //该终端对应的第一个显示位，32kb的显存可供8个屏幕显示
    uint32_t display_rows;   //显示的行数
    uint32_t display_cols;  //显示的列数
    uint32_t cursor_row;    //当前光标所在行
    uint32_t cursor_col;    //当前光标所在列
    color_t foreground;     //前景色
    color_t background;     //背景色
    uint32_t old_cursor_col;    //保存的光标所在行
    uint32_t old_cursor_row;    //保存的光标所在列

}console_t;


int console_init(void);
int console_write(int console, char *data, int size);
void console_close(int console);

#endif