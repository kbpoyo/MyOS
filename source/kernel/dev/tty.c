/**
 * @file tty.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief tty设备
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/tty.h"
#include "dev/dev.h"
#include "tools/log.h"
#include "dev/keyboard.h"
#include "dev/console.h"

static tty_t tty_table[TTY_TABLE_SIZE];


static void tty_fifo_init(tty_fifo_t *fifo, char *buf, int size) {
    fifo->buf = buf;
    fifo->count = 0;
    fifo->size = size;
    fifo->read = fifo->write = 0;
}

/**
 * @brief 打开tty设备
 * 
 */
int tty_open(device_t *dev) {
    int index = dev->dev_code;
    if (index < 0 || index >= TTY_TABLE_SIZE) {
        log_printf("open tty failed. incorrect tty num = %d", index);
        return -1;
    }

    tty_t *tty = tty_table + index;
    //初始化输入输出缓冲队列
    tty_fifo_init(&tty->in_fifo, tty->in_buf, TTY_IBUF_SIZE);
    tty_fifo_init(&tty->out_fifo, tty->out_buf, TTY_OBUF_SIZE);

    //为tty设备绑定输出终端
    tty->console_index = index;

    //初始化tty设备需要的键盘与终端
    kbd_init();
    console_init(index);
    
    
    return 0;
}

/**
 * @brief 读取读取设备
 * 
 */
int tty_read(device_t *dev, int addr, char *buf, int size) {

    return 0;

}

/**
 * @brief 写入tty设备
 * 
 */
int tty_write(device_t *dev, int addr, char *buf, int size) {

    return 0;
} 
/**
 * @brief 向tty设备发送控制指令
 * 
 */
int tty_control(device_t *dev, int cmd, int arg0, int arg1) {

    return 0;
}

/**
 * @brief 关闭tty设备
 * 
 */
void tty_close(device_t *dev) {

}

//描述一个tty设备类型
dev_desc_t dev_tty_desc = {
    .dev_name = "tty",
    .dev_type = DEV_TTY,
    .open = tty_open,
    .read = tty_read,
    .write = tty_write,
    .control = tty_control,
    .close = tty_close
};