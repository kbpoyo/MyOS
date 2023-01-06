/**
 * @file loader_32.c
 * @author kbpoyo (kbpoyo.com)
 * @brief loader程序运行在保护模式下的部分 
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "loader.h"

/**
 * @brief  以LBA模式读取磁盘(有48位PIO与28位PIO, 此处使用48位PIO)
 * 
 * @param sector 读取的分区号
 * @param sector_count 读取的分区数量
 * @param buf 缓冲区第一个字节的地址
 */
static void read_disk(uint32_t sector, uint16_t sector_count, uint8_t *buf) {
    
    //1.设置以LBA模式进行读取，即将磁盘看作一片连续的扇区
    outb(0x1F6, 0xE0 | (0x0 << 4));             //0xE0 将寄存器第6位置1进入LBA模式，0x0将第4位置0指定驱动器号为主盘
                                                //现在一个通道上只有一个盘，默认当作主盘
                    
    //2.初始化各个端口寄存器的高8位
    outb(0x1F2, (uint8_t)(sector_count >> 8));  //读取扇区数的高8位
                                                //6字节LBA值，先初始化第456个字节
                                                //我暂时只用到了4个字节的LBA值, 所以第56个字节置0即可
    outb(0x1F3, (uint8_t)(sector >> 24));       //LBA4 
    outb(0x1F4, 0x00);                          //LBA5
    outb(0x1F5, 0x00);                          //LBA6

    //3.初始化各个端口寄存器的低8位
    outb(0x1F2, (uint8_t)sector_count);         //读取扇区数的低8位
    outb(0x1F3, (uint8_t)sector);               //LBA1
    outb(0x1F4, (uint8_t)(sector >> 8));        //LBA2
    outb(0x1F5, (uint8_t)(sector >> 16));       //LBA3

    //4.将读取扇区命令 （0x24） 发送到端口 0x1F7
    outb(0x1F7, 0x24);

    //5.读取状态端口寄存器，判断是否可读取,若可以则读取，否则阻塞等待
    uint16_t *data_buf = (uint16_t*) buf;       //数据缓冲区，以后每次会读取16位数据
    while (sector_count--) {
        while ((inb(0x1F7) & 0x88) != 0x8) {}; //取出状态寄存器3位和7位
                                                //若!=0x8即DRQ位(3位)为0，即非就绪状态
                                                //或者BSY(7位)为1，即忙碌状态，都不可读取
        
        for (int i = 0; i < SECTOR_SIZE / 2; ++i) {
            *(data_buf++) = inw(0x1F0);           //从数据端口寄存器中读取16位数据，即2个字节
        }
        
    }
    
}

/**
 * @brief  32位loader程序的入口函数
 * 
 */
void load_kernel(void) {
    read_disk(100, 500, (uint8_t*)SYS_KERNEL_LOAD_ADDR); //从磁盘100号分区读取内核，一共读取250kb，到
    ((void(*)(void))SYS_KERNEL_LOAD_ADDR)();
    for (;;){};
}