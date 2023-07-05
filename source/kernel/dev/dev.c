/**
 * @file dev.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 设备管理方法
 * @version 0.1
 * @date 2023-07-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/dev.h"
#include "cpu/idt.h"
#define DEV_TABLE_SIZE  128

//声明外部的tty设备描述结构
extern dev_desc_t dev_tty_desc;

//设备描述结构表，用来获取某一类型设备的操作方法
static dev_desc_t *dev_des_table[] = {
    &dev_tty_desc,
};

//设备表，用于获取特定设备
static device_t dev_table[DEV_TABLE_SIZE];



/**
 * @brief 打开一个设备
 * 
 * @param dev_type 打开的设备类型
 * @param dev_code 打开的对应类型设备中的具体设备号
 * @param data 设备需要的参数数据
 * @return int 返回dev_id，该设备的描述符
 */
int dev_open(int dev_type, int dev_code, void *data) {
    //1.关中断，确保线程成功获取到设备描述符
    idt_state_t state = idt_enter_protection();
    
    //2.遍历dev_table，若该设备打开过则记录打开次数再返回描述符
    //若未打开过则在dev_table分配空间给该设备，并打开该设备
    device_t *free_dev = (device_t*)0;
    for (int i = 0; i < sizeof(dev_table) / sizeof(dev_table[0]); ++i) {
        device_t *dev = dev_table + i;
        if (dev->open_count == 0) { 
            //需要打开的设备未打开过，为该设备分配空间
            free_dev = dev;
        } else if (dev->desc->dev_type == dev_type && dev->dev_code == dev->dev_code) {
            //设备已打开过,增加打开次数
            dev->open_count++;
            idt_leave_protection(state);
            //返回设备描述符
            return i;
        }
    }

    //3.设备未打开过，遍历设备类型描述表，寻找对应的设备类型用于初始化该设备
    dev_desc_t *desc = (dev_desc_t*)0;
    for (int i = 0; i < sizeof(dev_des_table) / sizeof(dev_des_table[0]); ++i) {
        dev_desc_t *d = dev_des_table[i];
        if (d->dev_type == dev_type) {
            desc = d;
            break;
        }
    }

    //4.设备描述结构存在，设备空间分配成功，进行设备的初始化
    if (desc && free_dev) {
        free_dev->desc = desc;
        free_dev->data = data;
        free_dev->dev_code = dev_code;

        //用该设备描述结构打开该设备
        int err = desc->open(free_dev);
        if (err == 0) { //打开成功
            free_dev->open_count = 1;
            idt_leave_protection(state);
            //返回设备描述符
            return free_dev - dev_table;

        }
    }

    idt_leave_protection(state);
    return -1;
}


/**
 * @brief 读取设备
 * 
 * @param dev_id 设备描述符
 * @param addr 读取地址
 * @param buf 读取数据缓冲区
 * @param size 读取字节大小
 * @return int 成功读取大小
 */
int dev_read(int dev_id, int addr, char *buf, int size) {
    return size;
}

/**
 * @brief 写入设备
 * 
 * @param dev_id 设备描述符
 * @param addr 写入地址
 * @param buf 写入数据缓冲区
 * @param size 写入字节大小
 * @return int 成功写入大小
 */
int dev_write(int dev_id, int addr, char *buf, int size)  {
    return size;
}

/**
 * @brief 向设备发送控制指令
 * 
 * @param dev_id 设备描述符
 * @param cmd 控制指令
 * @param arg0 指令参数
 * @param arg1 指令参数
 * @return int 
 */
int dev_control(int dev_id, int cmd, int arg0, int arg1) {
    return 0;
}

/**
 * @brief 关闭设备
 * 
 * @param dev_id 设备描述符
 */
void dev_close(int dev_id) {

}