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

/**
 * @brief 打开tty设备
 * 
 */
int tty_open(device_t *dev) {
    
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