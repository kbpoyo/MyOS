/**
 * @file console.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 控制台设备
 * @version 0.1
 * @date 2023-06-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/console.h"

#define CONSOLE_NR  1//控制台个数

static console_t console_buf[CONSOLE_NR];//控制台对象数组


/**
 * @brief 将控制台console的光标迁移n个位置
 * 
 * @param console 
 * @param n 
 */
static inline void move_forward(console_t *console, int n) {
    for (int i = 0; i < n; ++i) {
        if (++(console->cursor_col) >= console->display_cols) { //光标达到最大列数，进行换行
            console->cursor_row++;
            console->cursor_col = 0;
        }
    }
}

/**
 * @brief 将字符c显示到控制台console上
 * 
 * @param console 
 * @param c 
 */
static inline void show_char(console_t *console, char c) {
    //计算当前光标对应控制台在现存位置中的偏移量
    int offset = console->cursor_col + console->cursor_row * console->display_cols;
    disp_char_t *p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_forward(console, 1);
}

/**
 * @brief 初始化控制台
 * 
 * @return int 
 */
int console_init(void) {
    for (int i = 0; i < CONSOLE_NR; ++i) {
        console_t *console = console_buf + i;
        console->display_rows = CONSOLE_ROW_MAX;
        console->display_cols = CONSOLE_CLO_MAX;
        console->foreground = COLOR_White;
        console->background = COLOR_Black;
        
        //计算每个终端在现存中的起始地址
        console->disp_base = (disp_char_t*)CONSOLE_DISP_START_ADDR + (i * CONSOLE_CLO_MAX * CONSOLE_ROW_MAX);
    }

    return 0;
}

/**
 * @brief 向console控制台写入data字符串
 * 
 * @param console 第几个控制台
 * @param data 写入的字符串
 * @param size 字符串大小
 * @return int 
 */
int console_write(int console, char *data, int size) {
    //获取需要需要写入的终端
    console_t *c = console_buf + console;
    int len;
    for (len = 0; len < size; ++len) {
        char ch = *(data++);
        show_char(c, ch);
    }

    return len;
}

/**
 * @brief 关闭控制台
 * 
 * @param console 
 * @return int 
 */
void console_close(int console) {

}