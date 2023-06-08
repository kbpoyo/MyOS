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

#include "common/cpu_instr.h"
#include "tools/klib.h"

#define CONSOLE_NR 1  // 控制台个数

static console_t console_buf[CONSOLE_NR];  // 控制台对象数组


/**
 * @brief 获取光标位置
 *
 * @return int
 */
static inline int read_cursor_pos(void) {
  int pos;
  // 光标位置由两个字节组成
  outb(0x3d4, 0xf);  // 访问低8位
  pos = inb(0x3d5);

  outb(0x3d4, 0xe);  // 访问高8位
  pos |= inb(0x3d5) << 8;

  return pos;
}

/**
 * @brief 更新光标的位置
 *
 * @param console
 * @return int
 */
static inline int update_cursor_pos(console_t *console) {
  uint16_t pos =
      console->cursor_row * console->display_cols + console->cursor_col;

  // 光标位置由两个字节组成
  outb(0x3d4, 0xf);  // 访问低8位
  outb(0x3d5, (uint8_t)(pos & 0xff));

  outb(0x3d4, 0xe);  // 访问高8位
  outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));

  return pos;
}

/**
 * @brief 擦除控制台的[start, end]行
 *
 * @param console
 * @param start
 * @param end
 */
static inline void erase_rows(console_t *console, int start, int end) {
  disp_char_t *disp_start = console->disp_base + console->display_cols * start;
  disp_char_t *disp_end =
      console->disp_base + console->display_cols * (end + 1);

  while (disp_start < disp_end) {
    disp_start->c = ' ';
    disp_start->foreground = console->foreground;
    disp_start->background = console->background;

    disp_start++;
  }
}

/**
 * @brief 控制台console上滚lines行
 *
 * @param console
 * @param lines
 */
static inline void scroll_up(console_t *console, int lines) {
  // 获取拷贝的目标位置
  disp_char_t *dest = console->disp_base;

  // 获取拷贝的源位置
  disp_char_t *src = console->disp_base + console->display_cols * lines;

  // 获取拷贝的字节量
  uint32_t size = (console->display_rows - lines) * console->display_cols *
                  sizeof(disp_char_t);

  // 拷贝
  kernel_memcpy(dest, src, size);

  // 清除底部重复行
  erase_rows(console, console->display_rows - lines, console->display_rows - 1);

  // 光标回退到之前的最后一行的下一行
  console->cursor_row -= lines;
}

/**
 * @brief 将控制台光标移动到第0列
 *
 * @param console
 */
static inline void move_to_col0(console_t *console) { console->cursor_col = 0; }

/**
 * @brief 将控制台光标移动到下一行
 *
 * @param console
 */
static inline void move_to_next_line(console_t *console) {
  console->cursor_row++;
  // 当前行已经大于最大显示行数，屏幕需要进行上滚1行
  if (console->cursor_row >= console->display_rows) {
    scroll_up(console, 1);
  }
}

/**
 * @brief 将控制台console的光标迁移n个位置
 *
 * @param console
 * @param n
 */
static inline void move_forward(console_t *console, int n) {
  for (int i = 0; i < n; ++i) {
    if (++(console->cursor_col) >=
        console->display_cols) {  // 光标达到最大列数，进行换行
      console->cursor_row++;
      console->cursor_col = 0;
      // 当前行已经大于最大显示行数，屏幕需要进行上滚1行
      if (console->cursor_row >= console->display_rows) {
        scroll_up(console, 1);
      }
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
  // 计算当前光标对应控制台在现存位置中的偏移量
  int offset =
      console->cursor_col + console->cursor_row * console->display_cols;
  disp_char_t *p = console->disp_base + offset;
  p->c = c;
  p->foreground = console->foreground;
  p->background = console->background;
  move_forward(console, 1);
}

/**
 * @brief 将光标左移n位
 * 
 * @param console 
 * @param n 
 * @return int 
 */
static inline int move_backword(console_t *console, int n) {
  int status = -1;
  for (int i = 0; i < n; ++i) {
    if (console->cursor_col > 0) {  //当前光标在行中，直接左移
      console->cursor_col--;
      status = 0;
    } else if (console->cursor_row > 0) { //当前光标在行开头，若当前行不为第一行则可移动到上一行末尾
      console->cursor_row--;
      console->cursor_col = console->display_cols - 1;
      status = 0;
    }
  }

  return status;
}


/**
 * @brief 往左擦除一个字符
 * 
 * @param console 
 */
static inline void erase_backword(console_t *console) {
  if (move_backword(console, 1) == 0) { //左移成功
    show_char(console, ' ');    //用空格覆盖需要擦除的字符
    move_backword(console, 1);  //再左移一位实现擦除
  }
}



/**
 * @brief 清空屏幕
 *
 * @param console
 */
static inline void clear_display(console_t *console) {
  int size = console->display_cols * console->display_rows;
  disp_char_t *start = console->disp_base;
  for (int i = 0; i < size; ++i, ++start) {
    start->c = ' ';
    start->foreground = console->foreground;
    start->background = console->background;
  }
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

    // 初始化光标位置
    int cursor_pos = read_cursor_pos();
    console->cursor_row = cursor_pos / console->display_cols;
    console->cursor_col = cursor_pos % console->display_cols;
    console->old_cursor_col = console->cursor_col;
    console->old_cursor_row = console->cursor_row;

    //初始化终端写入的状态
    console->write_state = CONSOLE_WRITE_NORMAL;

    // 计算每个终端在现存中的起始地址
    console->disp_base = (disp_char_t *)CONSOLE_DISP_START_ADDR +
                         (i * CONSOLE_CLO_MAX * CONSOLE_ROW_MAX);
  }

  return 0;
}



/**
 * @brief 终端写普通字符的策略
 * 
 * @param console 
 * @param c 
 */
static inline void write_normal(console_t *console, char c) {
   switch (c) {
      case ASCII_ESC:
        console->write_state = CONSOLE_WRITE_ESC;
        break;
      case 0x7f:  //退格
        erase_backword(console);
        break;
      case '\b':  //光标左移一位
        move_backword(console, 1);
        break;
      
      case '\r':  //回车
        move_to_col0(console);
        break;
      case '\n':  //换行
        move_to_col0(console);
        move_to_next_line(console);
        break;
      default:
        if (c >= ' ' && c <= '~') {// 可显示字符的范围
          show_char(console, c);
        }
        break;
    }
}


/**
 * @brief 保存光标当前位置
 * 
 * @param console 
 */
static inline void save_cursor(console_t *console) {
  console->old_cursor_col = console->cursor_col;
  console->old_cursor_row = console->cursor_row;
}

/**
 * @brief 恢复光标位置
 * 
 * @param console 
 */
static inline void restore_cursor(console_t *console) {
  console->cursor_col = console->old_cursor_col;
  console->cursor_row = console->old_cursor_row;
}

/**
 * @brief 终端写ESC字符的策略
 * 
 * @param console 
 * @param c 
 */
static inline void write_esc(console_t *console, char c) {
  switch (c) {
  case '7':
    save_cursor(console);
    console->write_state = CONSOLE_WRITE_NORMAL;
    break;
  case '8':
    restore_cursor(console);
    console->write_state = CONSOLE_WRITE_NORMAL;
  default:
    console->write_state = CONSOLE_WRITE_NORMAL;
    break;
  }
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
  // 获取需要需要写入的终端
  console_t *c = console_buf + console;
  int len;
  for (len = 0; len < size; ++len) {
    char ch = *(data++);
    switch (c->write_state) {
    case CONSOLE_WRITE_NORMAL:
      write_normal(c, ch);
      break;
    case CONSOLE_WRITE_ESC:
      write_esc(c, ch);
      break; 
    default:
      break;
    }
   
  }

  // 更新光标的位置
  update_cursor_pos(c);
  return len;
}

/**
 * @brief 关闭控制台
 *
 * @param console
 * @return int
 */
void console_close(int console) {}