/**
 * @file tty.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief tty设备,键盘和显示器的统一抽象，键盘只读，显示器只写
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TTY_H
#define TTY_H

//tty缓存队列
typedef struct _tty_fifo_t {
    char *buf;  //缓冲区起始地址
    int size;   //缓冲区大小
    int read, write;    //缓冲区读写位置
    int count;  //缓冲区有效数据大小，即未读数据大小
}tty_fifo_t;



#define TTY_TABLE_SIZE  8   //tty设备表的大小
#define TTY_OBUF_SIZE   512 //输出缓存大小
#define TTY_IBUF_SIZE   512 //输入缓存大小
//tty设备结构
typedef struct _tty_t {
    int console_index;  //tty对应的终端的索引
    tty_fifo_t out_fifo;    //输出缓存队列
    tty_fifo_t in_fifo;     //输入缓存队列
    char out_buf[TTY_OBUF_SIZE];    //输入缓存
    char in_buf[TTY_IBUF_SIZE];     //输出缓存
}tty_t;


#endif