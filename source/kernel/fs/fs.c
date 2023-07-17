/**
 * @file fs.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 文件操作
 * @version 0.1
 * @date 2023-05-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "fs/fs.h"
#include "fs/file.h"
#include "tools/klib.h"
#include "common/cpu_instr.h"
#include "common/boot_info.h"
#include "dev/console.h"
#include "dev/dev.h"
#include "core/task.h"
#include "tools/log.h"

//定义缓冲区位置，用于暂存从磁盘中读取的文件内容
#define TEMP_ADDR   (120*1024*1024)

//定义一个文件暂时的文件描述符值
#define TEMP_FILE_ID    100

//定义文件读取的指针，记录当前读到哪一个字节
static uint8_t *temp_pos;


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
 * @brief 判断文件路径是否有效
 * 
 * @param path 
 * @return int 
 */
static int is_path_valid(const char *path) {
    if (path == (const char*)0 || path[0] == '\0') {//路径无效
        return 0;
    }

    return 1;
}

/**
 * @brief 打开文件
 * 
 * @param name 文件路径
 * @param flags 打开方式的标志
 * @param ... 
 * @return int 文件描述符 
 */
int sys_open(const char *name, int flags, ...) {
    if (kernel_strncmp(name, "tty", 3) == 0) {  //打开tty设备文件
        
        //1.判断路径是否有效
        if(!is_path_valid(name)) {//文件路径无效
            log_printf("path is not valid\n");
            return -1;
        }

        //2.从系统file_table中分配一个文件结构
        file_t *file = file_alloc();

        //3.将文件结构放入当前进程的打开文件表中并得到文件描述符
        int fd = -1;
        if (file) {//分配成功，放入调用该系统调用的任务进程的打开文件表中
            fd = task_alloc_fd(file);
            if (fd < 0) {//放入失败
                goto sys_open_failed;
            }
        } else {//分配失败
            goto sys_open_failed;
        }

        //4.打开对应具体设备号的tty设备
        //name的开头为"tty:0(设备号)"
        int num = name[4] - '0';    //获取设备号
        int dev_id = dev_open(DEV_TTY, num, 0); //打开设备并获取设备描述符
        if (dev_id < 0) {
            goto sys_open_failed;
        }

        //5.将打开的tty设备与分配的文件结构绑定
        file->dev_id = dev_id;
        file->mode = 0;
        file->pos = 0;
        file->ref = 1;
        file->type = FILE_TTY;
        kernel_strncpy(file->file_name, name, FILE_NAME_SIZE);


        //6.返回文件描述符
        return fd;

    //打开失败，回收资源
    sys_open_failed:
        if (file) { //释放掉已分配的file
            file_free(file);
        }
        if (fd >= 0) {  //释放掉已分配的打开文件表的资源
            task_remove_fd(fd);
        }

        return -1;

    } else if (name[0] == '/') {  //打开外部程序
        read_disk(5000, 80, (uint8_t*)TEMP_ADDR);
        temp_pos = (uint8_t*)TEMP_ADDR;
        return TEMP_FILE_ID;
    }

    return -1;
}
/**
 * @brief 读文件
 * 
 * @param file 文件描述符
 * @param ptr 缓冲区地址
 * @param len 读取字节数
 * @return int 成功读取字节数
 */
int sys_read(int file, char *ptr, int len) {
    if (file == TEMP_FILE_ID) {
        kernel_memcpy(ptr, temp_pos, len);
        temp_pos += len;
        return len;
    } else {
        //根据文件描述符从当前进程的打开文件表中获取文件指针
        file_t *p_file = task_file(file);
        if (!p_file) {//获取失败
            log_printf("file not opened!\n");
            return -1;
        }
        //2.对文件结构所对应的设备进行真实的读操作
        return dev_read(p_file->dev_id, 0, ptr, len);
    }
    return -1;
}

/**
 * @brief 写文件
 * 
 * @param file 文件描述符
 * @param ptr 缓冲区地址
 * @param len 写入字节数
 * @return int 成功写入字节数
 */
int sys_write(int file, char *ptr, int len) {
    //1.根据文件描述符从当前进程的打开文件表中获取文件结构指针
     file_t *p_file = task_file(file);

    if (!p_file) {//获取失败
        log_printf("file not opened!\n");
        return -1;
    }

    //2.对文件结构所对应的设备进行真实的写操作
    return dev_write(p_file->dev_id, 0, ptr, len);

    // if (file == 1) {
    //     ptr[len] = '\0';
    //     log_printf("%s", ptr);
    //     //console_write(0, ptr, len);
       
    //     // int dev_id = dev_open(DEV_TTY, 0, (void*)0);
    //     // dev_write(dev_id, 0, ptr, len);
    //     // dev_close(dev_id);

    // }
    // return -1;
}

/**
 * @brief 使文件读取位置从文件头偏移offset个字节
 * 
 * @param file 
 * @param offset 
 * @param pos 
 * @return int 
 */
int sys_lseek(int file, int offset, int pos) {
    if (file == TEMP_FILE_ID) {
        temp_pos = (uint8_t*)(TEMP_ADDR + offset);
        return 0;
    }

    return -1;

}

/**
 * @brief 关闭文件
 * 
 * @param file 
 * @return int 
 */
int sys_close(int file) {
    return 0;
}

/**
 * @brief 
 * 
 * @param file 
 * @return int 
 */
int sys_isatty(int file) {
    return -1;
}

/**
 * @brief 
 * 
 * @param file 
 * @param st 
 * @return int 
 */
int sys_fstat(int file, struct stat *st) {
    return -1;
}

/**
 * @brief 在当前进程的打开文件表中分配新的一项指向该文件描述符对应的文件指针
 * 
 * @param file 需要被多次引用的文件指针的文件描述符
 * @return int 新的文件描述符
 */
int sys_dup(int file) {
    if (file < 0 || file >= TASK_OFILE_SIZE) {
        log_printf("file %d is not valid.", file);
        return -1;
    }

    //1.获取需要重复引用的文件指针
    file_t *p_file = task_file(file);
    if (!p_file) {
        log_printf("file not opend!\n");
        return -1;
    }

    //2.在打开文件表中新分配一项给该文件指针
    int fd = task_alloc_fd(p_file);
    if (fd < 0) {
        log_printf("no task file avaliable\n");
        return -1;
    }

    p_file->ref++;  //分配成功，该文件引用次数加一
    return fd;
}

/**
 * @brief 初始化文件系统
 * 
 */
void fs_init(void) {
    file_table_init();
}