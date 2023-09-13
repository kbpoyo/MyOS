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
#include "tools/klib.h"

//定义设备表大小
#define DEV_TABLE_SIZE  128

//声明外部的disk设备描述结构
extern dev_desc_t dev_disk_desc;
//声明外部的tty设备描述结构
extern dev_desc_t dev_tty_desc;

//设备描述结构表，用来获取某一类型设备的操作方法
static dev_desc_t *dev_des_table[] = {
    [DEV_TTY] = &dev_tty_desc,
    [DEV_DISK] = &dev_disk_desc,
};

//设备表，用于获取特定设备
static device_t dev_table[DEV_TABLE_SIZE];


/**
 * @brief 根据设备id判断设备是否存在
 * 
 * @param dev_id 
 * @return int 
 */
static int is_dev_exist(int dev_id) {
    //dev_id越界，设备不存在
    if (dev_id < 0 || dev_id >= DEV_TABLE_SIZE) {
        return 0;
    }
    
    //设备空间未初始化，设备不存在
    if (dev_table[dev_id].open_count == 0 || dev_table[dev_id].desc == (dev_desc_t*)0) {
        return 0;
    }

    return 1;
}


/**
 * @brief 打开一个设备
 * 
 * @param dev_type 打开的设备类型
 * @param dev_index 打开的对应类型设备中的具体设备号
 * @param data 设备需要的参数数据
 * @return int 返回dev_id，该设备的描述符
 */
int dev_open(int dev_type, int dev_index, void *data) {
    //TODO:加锁 1.关中断，确保线程成功获取到设备描述符
    idt_state_t state = idt_enter_protection();
    
    //2.遍历dev_table，若该设备打开过则记录打开次数再返回描述符
    //若未打开过则在dev_table分配空间给该设备，并打开该设备
    device_t *free_dev = (device_t*)0;
    for (int i = 0; i < sizeof(dev_table) / sizeof(dev_table[0]); ++i) {
        device_t *dev = dev_table + i;
        if (dev->open_count == 0) { 
            //需要打开的设备未打开过，为该设备分配空间
            free_dev = dev;
            break;
        } else if (dev->dev_type == dev_type && dev->dev_index == dev_index) {
            //设备已打开过,增加打开次数
            dev->open_count++;
            //TODO:解锁
            idt_leave_protection(state);
            //返回设备描述符
            return i;
        }
    }

    //3.设备描述结构存在，设备空间分配成功，进行设备的初始化
    if (free_dev && dev_type > DEV_UNKNOWN && dev_type < sizeof(dev_des_table) / sizeof(dev_des_table[0])) {
        free_dev->dev_type = dev_type;
        free_dev->desc = dev_des_table[dev_type];
        free_dev->data = data;
        free_dev->dev_index = dev_index;

        //用该设备描述结构打开该设备
        int err = free_dev->desc->open(free_dev);
        if (err == 0) { //打开成功
            free_dev->open_count = 1;
            //TODO:解锁
            idt_leave_protection(state);
            //返回设备描述符
            return free_dev - dev_table;

        }
    }
  
    //TODO:解锁
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
    //设备不存在，直接返回-1
    if (!is_dev_exist(dev_id)) {
        return -1;
    }

    //获取设备，并通过其描述结构的操作接口进行真正的读操作
    device_t *dev = dev_table + dev_id;
    return dev->desc->read(dev, addr, buf, size);
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
    //设备不存在，直接返回-1
    if (!is_dev_exist(dev_id)) {
        return -1;
    }

    //获取设备，并通过其描述结构的操作接口进行真正的写操作
    device_t *dev = dev_table + dev_id;
    return dev->desc->write(dev, addr, buf, size);
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
    //设备不存在，直接返回-1
    if (!is_dev_exist(dev_id)) {
        return -1;
    }

    //获取设备，并通过其描述结构的操作接口进行真正的控制操作
    device_t *dev = dev_table + dev_id;
    return dev->desc->control(dev, cmd, arg0, arg1);
}

/**
 * @brief 关闭设备
 * 
 * @param dev_id 设备描述符
 */
void dev_close(int dev_id) {
    //设备不存在，直接返回-1
    if (!is_dev_exist(dev_id)) {
        return;
    }

    //获取设备，并通过其描述结构的操作接口进行真正的关闭操作
    device_t *dev = dev_table + dev_id;
    
    //释放资源
    idt_state_t state = idt_enter_protection();
    if (--dev->open_count == 0) {   //该设备已不被操作系统引用，可以关闭
        //关闭设备
        dev->desc->close(dev);
        //释放设备空间资源
        kernel_memset(dev, 0, sizeof(device_t));
    }

    idt_leave_protection(state);
}

