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
#include "common/elf.h"

/**
 * @brief  以LBA模式读取磁盘(有48位PIO与28位PIO, 此处使用48位PIO)
 * 
 * @param sector 读取的分区号
 * @param sector_count 读取的分区数量
 * @param buf 缓冲区第一个字节的地址
 */
static void read_disk(uint32_t sector, uint16_t sector_count, uint8_t *buf) {
    
    //1.设置以LBA模式进行读取，即将磁盘看作一片连续的扇区
    outb(0x1F6, 0xE0 | (0x0 << 4));              //0xE0 将寄存器第6位置1进入LBA模式，0x0将第4位置0指定驱动器号为主盘
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
        while ((inb(0x1F7) & 0x88) != 0x8) {};  //取出状态寄存器3位和7位
                                                //若!=0x8即DRQ位(3位)为0，即非就绪状态
                                                //或者BSY(7位)为1，即忙碌状态，都不可读取

        for (int i = 0; i < SECTOR_SIZE / 2; ++i) {
            *(data_buf++) = inw(0x1F0);          //从数据端口寄存器中读取16位数据，即2个字节
        }
        
    }
    
}

/**
 * @brief   解析内存中地址为 file_start_addr 处的elf文件头，并将.text,.rodata,.data,.bss段
 *          拷贝到内存中的目的地址，elf文件会将(.text, .rodata)，(.data, .bss)各放在一个段
 * 
 * @param file_start_addr 已加载到内存中的elf文件的起始地址
 * @return uint32_t   返回所拷贝的程序段的入口地址
 */
static uint32_t reload_elf_file(uint8_t *file_start_addr ) {
    //1.强转为Elf32_Ehdr, 使该结构体可以访问到之后的52字节, 即elf header所包含的内容
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr*)file_start_addr; 

    //2.判断为访问的内存区域是否为elf文件,若不是则直接返回0
    if (elf_hdr->e_ident[0] != 0x7f 
        || elf_hdr->e_ident[1] != 'E' 
        || elf_hdr->e_ident[2] != 'L' 
        || elf_hdr->e_ident[3] != 'F') return 0;

    //3.解析program hear即段的头信息数组
    for (int i = 0; i < elf_hdr->e_phnum; ++i) {
        //4.以 Elf32_Phdr 内存大小为单位，逐个读取段的头信息，e_phoff 为其起始地址相对于elf文件的偏移量
        Elf32_Phdr *phdr = (Elf32_Phdr*)(file_start_addr + elf_hdr->e_phoff) + i;

        //5.判断是否为可加载的程序段, 不是则直接忽略
        if (phdr->p_type != PT_LOAD) continue;

        //6.找到可加载程序段在内存中的位置, p_offset为其起始地址相对于elf文件的偏移量
        uint8_t *src = file_start_addr + phdr->p_offset;

        //7.加载到内存中的目的地址
        uint8_t *dest = (uint8_t*)phdr->p_vaddr;

        //8.逐个字节拷贝到对应位置
        for (int i = 0; i < phdr->p_filesz; ++i) {
            *(dest++) = *(src++);
        }

        //9.若加载的是.data段和.bss段，则.bss段因为未初始化，所以在elf文件中并为为其预留空间
        //但在内存中需要为其分配空间，还需要分配空间大小为 (p_memsz - p_filesz),置0即可
        for (int i = 0; i < phdr->p_memsz - phdr->p_filesz; ++i) {
            *(dest++) = 0;
        }
    }

    //10.返回解析的程序段的入口地址
    return elf_hdr->e_entry;
}

/**
 * @brief  读取错误代码进行异常处理
 * 
 * @param err_code 
 */
static void die(uint8_t err_code) {
    //TODO:暂时什么都不做，直接卡死电脑
    for(;;){}
}

/**
 * @brief  32位loader程序的入口函数
 * 
 */
void load_kernel(void) {
    //1.从磁盘100号分区读取内核，一共读取250kb，到内存中地址为SYS_KERNEL_LOAD_ADDR的地方
    read_disk(100, 500, (uint8_t*)SYS_KERNEL_LOAD_ADDR);            

    //2.解析内存中地址为SYS_KERNEL_LOAD_ADDR处的elf文件头     
    uint32_t kernel_entry = reload_elf_file((uint8_t*)SYS_KERNEL_LOAD_ADDR); 
    
    //3.若函数执行失败，返回的入口地址为0，进行错误处理
    if (kernel_entry == 0) { 
        die(-1);
    }               

    //4.将boot_info记录的信息传递给已拷贝到确定内存中的内核初始化函数
    ((void(*)(boot_info_t*))kernel_entry)(&boot_info);    
}