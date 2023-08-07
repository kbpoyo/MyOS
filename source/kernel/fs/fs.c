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

#include "common/boot_info.h"
#include "common/cpu_instr.h"
#include "core/task.h"
#include "dev/console.h"
#include "dev/dev.h"
#include "fs/file.h"
#include "tools/klib.h"
#include "tools/list.h"
#include "tools/log.h"

#define FS_TABLE_SIZE 10
static list_t mounted_list;           // 文件系统挂载链表
static fs_t fs_table[FS_TABLE_SIZE];  // 全局文件系统表
static list_t free_list;              // 存放未挂载的文件系统
extern fs_op_t devfs_op;

// 定义缓冲区位置，用于暂存从磁盘中读取的文件内容
#define TEMP_ADDR (120 * 1024 * 1024)

// 定义一个文件暂时的文件描述符值
#define TEMP_FILE_ID 100

// 定义文件读取的指针，记录当前读到哪一个字节
static uint8_t *temp_pos;

/**
 * @brief  以LBA模式读取磁盘(有48位PIO与28位PIO, 此处使用48位PIO)
 *
 * @param sector 读取的分区号
 * @param sector_count 读取的分区数量
 * @param buf 缓冲区第一个字节的地址
 */
static void read_disk(uint32_t sector, uint16_t sector_count, uint8_t *buf) {
  // 1.设置以LBA模式进行读取，即将磁盘看作一片连续的扇区
  outb(
      0x1F6,
      0xE0 |
          (0x0
           << 4));  // 0xE0
                    // 将寄存器第6位置1进入LBA模式，0x0将第4位置0指定驱动器号为主盘
                    // 现在一个通道上只有一个盘，默认当作主盘

  // 2.初始化各个端口寄存器的高8位
  outb(0x1F2,
       (uint8_t)(sector_count >>
                 8));  // 读取扇区数的高8位
                       // 6字节LBA值，先初始化第456个字节
                       // 我暂时只用到了4个字节的LBA值, 所以第56个字节置0即可
  outb(0x1F3, (uint8_t)(sector >> 24));  // LBA4
  outb(0x1F4, 0x00);                     // LBA5
  outb(0x1F5, 0x00);                     // LBA6

  // 3.初始化各个端口寄存器的低8位
  outb(0x1F2, (uint8_t)sector_count);    // 读取扇区数的低8位
  outb(0x1F3, (uint8_t)sector);          // LBA1
  outb(0x1F4, (uint8_t)(sector >> 8));   // LBA2
  outb(0x1F5, (uint8_t)(sector >> 16));  // LBA3

  // 4.将读取扇区命令 （0x24） 发送到端口 0x1F7
  outb(0x1F7, 0x24);

  // 5.读取状态端口寄存器，判断是否可读取,若可以则读取，否则阻塞等待
  uint16_t *data_buf = (uint16_t *)buf;  // 数据缓冲区，以后每次会读取16位数据
  while (sector_count--) {
    while ((inb(0x1F7) & 0x88) != 0x8) {
    };  // 取出状态寄存器3位和7位
        // 若!=0x8即DRQ位(3位)为0，即非就绪状态
        // 或者BSY(7位)为1，即忙碌状态，都不可读取

    for (int i = 0; i < SECTOR_SIZE / 2; ++i) {
      *(data_buf++) = inw(0x1F0);  // 从数据端口寄存器中读取16位数据，即2个字节
    }
  }
}


/**
 * @brief 检验文件描述符fd是否有效
 * 
 * @param fd 
 * @return int 
 */
static int is_fd_bad(int fd) {
  if (fd < 0 && fd >= TASK_OFILE_SIZE) {
    return 1;
  }

  return 0;
}

/**
 * @brief 判断文件路径是否有效
 *
 * @param path
 * @return int
 */
static int is_path_valid(const char *path) {
  if (path == (const char *)0 || path[0] == '\0') {  // 路径无效
    return 0;
  }

  return 1;
}

/**
 * @brief 获取路径path下一级路径
 *
 * @param path
 * @return const char*
 */
const char *path_next_child(const char *path) {
  const char *c = path;
  while (*c && (*(c++) == '/')) {
  };  // 跳过第一个'/'
  while (*c && (*(c++) != '/')) {
  };  // 跳过一级目录到下一个'/'

  return *c ? c : (const char *)0;  // 返回第二个'/'后的部分
}

/**
 * @brief 从路径path中提取次设备号到num中
 *
 * @param path
 * @param num
 * @return int
 */
int path_to_num(const char *path, int *num) {
  int n = 0;
  const char *c = path;

  while (*c) {
    n = n * 10 + *c - '0';
    c++;
  }

  *num = n;

  return n;
}

/**
 * @brief 判断路径path是否是以str开头
 * 
 * @param path 
 * @param str 
 * @return int 
 */
int path_begin_with(const char *path, const char *str) {
  const char *s1 = path, *s2 = str;
  while (*s1 && *s2 && (*s1 == *s2)) {
    s1++;
    s2++;
  }

  return *s2 == '\0';
}
/**
 * @brief 对文件系统的操作进行保护
 * 
 * @param fs 
 */
static void fs_protect(fs_t *fs) {
  if (fs->mutex) {
    mutex_lock(fs->mutex);
  }
}

/**
 * @brief 对文件系统的操作进行保护
 * 
 * @param fs 
 */
static void fs_unprotect(fs_t *fs) {
  if (fs->mutex) {
    mutex_unlock(fs->mutex);
  }
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
  // 1.判断路径是否有效
  if (!is_path_valid(name)) {  // 文件路径无效
    log_printf("path is not valid\n");
    return -1;
  }

  if (kernel_strncmp(name, "/shell.elf", 10) == 0) {
    // 打开外部程序
    read_disk(5000, 80, (uint8_t *)TEMP_ADDR);
    temp_pos = (uint8_t *)TEMP_ADDR;
    return TEMP_FILE_ID;
  }

  // 2.从系统file_table中分配一个文件结构
  file_t *file = file_alloc();
  if (!file) {
    return -1;
  }
  // 3.将文件结构放入当前进程的打开文件表中并得到文件描述符
  int fd = task_alloc_fd(file);
  if (fd < 0) {  // 放入失败
    goto sys_open_failed;
  }

  //遍历文件系统挂载链表mounted_list,寻找需要打开的文件对应的文件系统
  fs_t *fs = (fs_t *)0;
  list_node_t *node = list_get_first(&mounted_list);
  while (node) {
    fs_t *curr = list_node_parent(node, fs_t, node);
    if (path_begin_with(name, curr->mount_point)) { //该文件属于curr这个文件系统
      fs = curr;
      break;
    }

    node = list_node_next(node);
  }


  if (fs) { //找到对应的文件系统
    // 获取下一级路径
    name = path_next_child(name);
  } else {  //未找到对应文件系统，使用默认文件系统

  }

  //为文件绑定模式参数和文件系统  
  file->mode = flags;
  file->fs = fs;
  kernel_strncpy(file->file_name, name, FILE_NAME_SIZE);

  //使用该文件系统打开该文件
  fs_protect(fs);
  int err = fs->op->open(fs, name, file);
  fs_unprotect(fs);

  if (err < 0) {
    log_printf("打开失败!");
    goto sys_open_failed;
  }

  // 6.返回文件描述符
  return fd;

// 打开失败，回收资源
sys_open_failed:
  // 释放掉已分配的file
  file_free(file);
  if (fd >= 0) {  // 释放掉已分配的打开文件表的资源
    task_remove_fd(fd);
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
    // 根据文件描述符从当前进程的打开文件表中获取文件指针
    file_t *p_file = task_file(file);
    if (!p_file) {  // 获取失败
      log_printf("file not opened!\n");
      return -1;
    }
    // 2.对文件结构所对应的设备进行真实的读操作
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
  // 1.根据文件描述符从当前进程的打开文件表中获取文件结构指针
  file_t *p_file = task_file(file);

  if (!p_file) {  // 获取失败
    log_printf("file not opened!\n");
    return -1;
  }

  // 2.对文件结构所对应的设备进行真实的写操作
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
    temp_pos = (uint8_t *)(TEMP_ADDR + offset);
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
int sys_close(int file) { return 0; }

/**
 * @brief
 *
 * @param file
 * @return int
 */
int sys_isatty(int file) { return -1; }

/**
 * @brief
 *
 * @param file
 * @param st
 * @return int
 */
int sys_fstat(int file, struct stat *st) { return -1; }

/**
 * @brief 在当前进程的打开文件表中分配新的一项指向该文件描述符对应的文件指针
 *
 * @param fd 需要被多次引用的文件指针的文件描述符
 * @return int 新的文件描述符
 */
int sys_dup(int fd) {
  if (is_fd_bad(fd)) {
    log_printf("fd %d is not valid.", fd);
    return -1;
  }

  // 1.获取需要重复引用的文件指针
  file_t *file = task_file(fd);
  if (!file) {
    log_printf("file not opend!\n");
    return -1;
  }

  // 2.在打开文件表中新分配一项给该文件指针
  int new_fd = task_alloc_fd(file);
  if (new_fd < 0) {
    log_printf("no task file avaliable\n");
    return -1;
  }

  file_inc_ref(file);  // 分配成功，该文件引用次数加一
  return fd;
}

/**
 * @brief 初始化free_list和mount_list
 *
 */
static void mount_list_init(void) {
  list_init(&free_list);
  // 将全局文件系统加入到free_list，等待被挂载
  for (int i = 0; i < FS_TABLE_SIZE; ++i) {
    list_insert_first(&free_list, &(fs_table[i].node));
  }

  list_init(&mounted_list);
}

/**
 * @brief 根据文件系统类型获取对应的操作函数表
 *
 * @param type
 * @param major
 * @return fs_op_t*
 */
static fs_op_t *get_fs_op(fs_type_t type, int major) {
  switch (type) {
    case FS_DEVFS:
      return &devfs_op;
      break;
    default:
      return 0;
      break;
  }
}

/**
 * @brief 挂载文件系统
 *
 * @param type 文件系统类型
 * @param mount_point 文件系统的挂载点，即文件系统的名称
 * @param dev_major 具体文件的主设备号
 * @param dev_minor 具体文件的次设备号
 * @return fs_t*
 */
static fs_t *mount(fs_type_t type, const char *mount_point, int dev_major,
                   int dev_minor) {
  fs_t *fs = (fs_t *)0;
  log_printf("mount file system, name: %s, dev: %x", mount_point, dev_major);

  // 1.检查当前文件系统是否已被挂载
  list_node_t *curr = list_get_first(&mounted_list);
  while (curr) {
    fs_t *fs = list_node_parent(curr, fs_t, node);
    if (kernel_strncmp(fs->mount_point, mount_point, FS_MOUNT_POINT_SIZE) ==
        0) {
      log_printf("fs already mounted!");
      goto mount_failed;
    }

    curr = list_node_next(curr);
  }

  // 2.从空闲链表中取下一个待挂载的fs对象进行挂载
  list_node_t *free_node = list_remove_first(&free_list);
  if (!free_node) {
    log_printf("no free fs, mount failed!");
    goto mount_failed;
  }
  fs = list_node_parent(free_node, fs_t, node);
  kernel_memset(fs, 0, sizeof(fs_t));
  kernel_strncpy(fs->mount_point, mount_point, FS_MOUNT_POINT_SIZE);

  // 3.获取该fs对象的操作函数表并交给该对象
  fs_op_t *op = get_fs_op(type, dev_major);
  if (!op) {
    log_printf("unsupported fs type: %d", type);
    goto mount_failed;
  }
  fs->op = op;

  // 4.挂载该文件系统类型下具体的设备
  if (op->mount(fs, dev_major, dev_minor) < 0) {
    log_printf("mount fs %s failed!", mount_point);
    goto mount_failed;
  }

  // 5.将该文件系统挂载到mounted_list上
  list_insert_last(&mounted_list, &fs->node);

  return fs;

// 挂载失败的异常处理
mount_failed:
  // fs不为空，证明已从free_list上取下，需要将其插回去
  if (fs) {
    list_insert_last(&free_list, &fs->node);
  }
  return (fs_t *)0;
}

/**
 * @brief 初始化文件系统
 *
 */
void fs_init(void) {
  mount_list_init();
  file_table_init();

  fs_t *fs = mount(FS_DEVFS, "/dev", 0, 0);
  ASSERT(fs != (fs_t *)0);
}