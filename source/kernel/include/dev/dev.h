/**
 * @file dev.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 设备管理层接口
 * @version 0.1
 * @date 2023-07-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DEV_H
#define DEV_H


//定义设备类型枚举
enum {
    DEV_UNKNOWN = 0,
    DEV_TTY,    //TTY设备
    DEV_DISK,   //磁盘设备
};


struct _dev_desc_t;
//定义某种特定类型的硬件结构
typedef struct _device_t {
    struct _dev_desc_t *desc;   //设备类型描述结构
    int mode;
    int dev_index;          //设备号，指定某特定类型的设备中的某一个具体设备即作为具体的设备表中的索引
    void * data;        //存放设备相关数据
    int open_count;     //设备打开次数

}device_t;
int dev_open(int dev_type, int dev_code, void *data);
int dev_read(int dev_id, int addr, char *buf, int size);
int dev_write(int dev_id, int addr, char *buf, int size);
int dev_control(int dev_id, int cmd, int arg0, int arg1);
void dev_close(int dev_id);



#define DEV_NAME_SIZE   20
//定义设备类型描述结构,用来描述并操作各种类型的硬件，相当于接口类
typedef struct _dev_desc_t {
    char dev_name[DEV_NAME_SIZE];   //设备名称
    int dev_type;                      //指定设备类型

    int (*open)(device_t *dev);     //打开设备
    int (*read)(device_t *dev, int addr, char *buf, int size); //读取设备
    int (*write)(device_t *dev, int addr, char *buf, int size); //写入设备
    int (*control)(device_t *dev, int cmd, int arg0, int arg1); //向设备发送控制指令
    void (*close)(device_t *dev);   //关闭设备


}dev_desc_t;

#endif