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

/**
 * @brief 根据dev结构获取到对应的tty设备结构
 * 
 * @param dev 
 * @return tty_t* 
 */
static tty_t* get_tty(device_t *dev) {
    int tty_index = dev->dev_code;
    if (tty_index < 0 || tty_index >= TTY_TABLE_SIZE || !dev->open_count) {
        log_printf("tty is not opened. tty = %d", tty_index);
        return (tty_t*)0;
    }

    return tty_table + tty_index;
}

/**
 * @brief 初始化tty设备的缓冲队列
 * 
 * @param fifo 
 * @param buf 
 * @param size 
 */
static void tty_fifo_init(tty_fifo_t *fifo, char *buf, int size) {
    fifo->buf = buf;
    fifo->count = 0;
    fifo->size = size;
    fifo->read = fifo->write = 0;
}

/**
 * @brief 往缓冲队列fifo中写入字符c
 * 
 * @param fifo 
 * @param c 
 * @return int 
 */
int tty_fifo_put(tty_fifo_t *fifo, char c) {
    //fifo已满，不能再写入
    if (fifo->count >= fifo->size) {
        return -1;
    }

    fifo->buf[fifo->write++] = c;   //写入一个字符
    if (fifo->write >= fifo->size) { //循环队列的方式写入
        fifo->write = 0;
    }

    fifo->count++;
    return 0;
}

/**
 * @brief 从缓冲队列fifo中读取一个字符放到c中
 * 
 * @param fifo 
 * @param c 
 * @return int 
 */
int tty_fifo_get(tty_fifo_t *fifo, char *c) {
    if (fifo->count <= 0) {
        return -1;
    }

    *c = fifo->buf[fifo->read++];   //读取一个字符
    if (fifo->read >= fifo->size) { //循环队列的方式读取
        fifo->read = 0;
    }

    fifo->count--;

    return 0;
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
    tty_fifo_init(&tty->out_fifo, tty->out_buf, TTY_OBUF_SIZE);
    tty_fifo_init(&tty->in_fifo, tty->in_buf, 0);

    //初始化缓冲区的信号量, 缓冲区的每一个字节都视为资源
    sem_init(&tty->out_sem, TTY_OBUF_SIZE);
    sem_init(&tty->in_sem, 0);

    //为tty设备绑定输出终端
    tty->console_index = index;

    //初始化tty设备需要的键盘与终端
    kbd_init();
    console_init(index);
    
    
    return index;
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
    if (size < 0) {
        return -1;
    }

    //根据dev结构获取到对应的tty设备结构
    tty_t *tty = get_tty(dev);
    if (!tty) {
        return -1;
    }

    int len = 0;
    while (size) {
        //先获取到访问缓冲区一个字节资源的资格
        //若缓冲区写满就阻塞住，等待中断程序将缓冲区消耗掉再写
        sem_wait(&tty->out_sem);

        char c = *(buf++);
        int err = tty_fifo_put(&tty->out_fifo, c);
        if (err < 0) {
            break;
        }

        len++;
        size--;

        //显示器直接写显存，不需要写io端口
        //所以不需要交给中断处理程序，即当前进程自己往缓冲区写入并读取
        //此处是为了模仿当前进程对缓冲区写入的同时中断处理程序读取缓冲区
        //此处肯定不会阻塞在信号量中，但利用中断处理程序就会阻塞
        console_write(tty);
    }
    


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