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
#include "dev/disk.h"
#include "os_cfg.h"
#include <sys/file.h>

#define FS_TABLE_SIZE 10
static list_t mounted_list;           // 文件系统挂载链表
static fs_t fs_table[FS_TABLE_SIZE];  // 全局文件系统表
static list_t free_list;              // 存放未挂载的文件系统

//声明外部的文件系统类型的函数表
extern fs_op_t devfs_op;  //设备文件类型
extern fs_op_t fatfs_op;  //fat文件类型

//根文件系统
static fs_t *root_fs;



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

  // 遍历文件系统挂载链表mounted_list,寻找需要打开的文件对应的文件系统
  fs_t *fs = (fs_t *)0;
  list_node_t *node = list_get_first(&mounted_list);
  while (node) {
    fs_t *curr = list_node_parent(node, fs_t, node);
    if (path_begin_with(name,
                        curr->mount_point)) {  // 该文件属于curr这个文件系统
      fs = curr;
      break;
    }

    node = list_node_next(node);
  }

  if (fs) {  // 找到对应的文件系统
    // 获取下一级路径
    name = path_next_child(name);
  } else {  // 未找到对应文件系统，使用默认的根文件系统
    fs = root_fs;
  }

  // 为文件绑定模式参数和文件系统
  file->mode = flags;
  file->fs = fs;
  kernel_strncpy(file->file_name, name, FILE_NAME_SIZE);

  // 使用该文件系统打开该文件
  fs_protect(fs);
  int err = fs->op->open(fs, name, file);
  fs_unprotect(fs);

  if (err < 0) {
    log_printf("open failed!");
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
 * @param fd 文件描述符
 * @param buf 缓冲区地址
 * @param len 读取字节数
 * @return int 成功读取字节数
 */
int sys_read(int fd, char *buf, int len) {

  if (is_fd_bad(fd) || !buf || !len) {
    return -1;
  }

  //1.根据文件描述符从当前进程的打开文件表中获取文件指针
  file_t *file = task_file(fd);
  if (!file) {  // 获取失败
    log_printf("file not opened!\n");
    return -1;
  }

  //2.判断文件的读写模式
  if (file->mode == O_WRONLY) { //文件只写，不可读
    log_printf("file is write only!\n");
    return -1;
  }

  //3.获取文件对应的文件系统，并执行读操作
  fs_t *fs = file->fs;
  fs_protect(fs);
  int err = fs->op->read(buf, len, file);
  fs_unprotect(fs);

  return err;

}

/**
 * @brief 写文件
 *
 * @param fd 文件描述符
 * @param ptr 缓冲区地址
 * @param len 写入字节数
 * @return int 成功写入字节数
 */
int sys_write(int fd, char *buf, int len) {
   if (is_fd_bad(fd) || !buf || !len) {
    return -1;
  }

  //1.根据文件描述符从当前进程的打开文件表中获取文件指针
  file_t *file = task_file(fd);
  if (!file) {  // 获取失败
    log_printf("file not opened!\n");
    return -1;
  }

  //2.判断文件的读写模式
  if (file->mode == O_RDONLY) { //文件只读，不可写
    log_printf("file is read only!\n");
    return -1;
  }

  //3.获取文件对应的文件系统，并执行写操作
  fs_t *fs = file->fs;
  fs_protect(fs);
  int err = fs->op->write(buf, len, file);
  fs_unprotect(fs);
  
  return err;
}

/**
 * @brief 使文件读取位置从文件头偏移offset个字节
 *
 * @param fd
 * @param offset
 * @param pos
 * @return int
 */
int sys_lseek(int fd, int offset, int dir) {

  if (is_fd_bad(fd)) {
    return -1;
  }

  //1.根据文件描述符从当前进程的打开文件表中获取文件指针
  file_t *file = task_file(fd);
  if (!file) {  // 获取失败
    log_printf("file not opened!\n");
    return -1;
  }


  //2.获取文件对应的文件系统，并执行偏移操作
  fs_t *fs = file->fs;
  fs_protect(fs);
  int err = fs->op->seek(file, offset, dir);
  fs_unprotect(fs);
  
  return err;
}

/**
 * @brief 关闭文件
 *
 * @param fd
 * @return int
 */
int sys_close(int fd) {
  if (is_fd_bad(fd)) {
    log_printf("file error");
    return -1;
  }

  //1.从打开文件表中获取文件结构
  file_t *file = task_file(fd);
  if (!file) {
    log_printf("file not opened!\n");
    return -1;
  }

  ASSERT(file->ref > 0);  //文件必须为打开状态

  //2.若当前文件只被一个进程引用则获取对应文件系统并执行关闭操作
  if (file->ref-- == 1) {
    fs_t *fs = file->fs;
    fs_protect(fs);
    fs->op->close(file);
    fs_protect(fs);

    //关闭文件后释放文件结构
    file_free(file);
  }

  //3.当前文件还被其它进程所引用，只在当前进程的打开文件表中释放该文件即可
  task_remove_fd(fd);

  return 0;
}


/**
 * @brief
 *
 * @param fd
 * @param st
 * @return int
 */
int sys_fstat(int fd, struct stat *st) { 
  
   if (is_fd_bad(fd)) {
    log_printf("file error");
    return -1;
  }

  //1.从打开文件表中获取文件结构
  file_t *file = task_file(fd);
  if (!file) {
    log_printf("file not opened!\n");
    return -1;
  }

  //2.获取对应文件系统进行状态获取操作
  fs_t *fs = file->fs;
  kernel_memset(st, 0, sizeof(struct stat));
  fs_protect(fs);
  int err = fs->op->stat(file, st);
  fs_unprotect(fs);


  return err;
}

/**
 * @brief
 *
 * @param fd
 * @return int
 */
int sys_isatty(int fd) { 
   if (is_fd_bad(fd)) {
    log_printf("file error");
    return -1;
  }

  //1.从打开文件表中获取文件结构
  file_t *file = task_file(fd);
  if (!file) {
    log_printf("file not opened!\n");
    return -1;
  }

  return file->type == FILE_TTY;
}


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
 * @brief 根据path打开目录
 * 
 * @param path 
 * @param dir 
 * @return int 
 */
int sys_opendir(const char *path, DIR *dir) {
   // 使用该文件系统打开该目录
  fs_protect(root_fs);
  int err = root_fs->op->opendir(root_fs, path, dir);
  fs_unprotect(root_fs);
  return err;
}

/**
 * @brief 从目录dir中扫描读取目录项到dirent中
 * 
 * @param dir 
 * @param dirent 
 * @return int 
 */
int sys_readdir(DIR *dir, struct dirent *dirent) {
   // 使用该文件系统遍历该目录
  fs_protect(root_fs);
  int err = root_fs->op->readdir(root_fs, dir, dirent);
  fs_unprotect(root_fs);
  return err;
}

/**
 * @brief 关闭目录
 * 
 * @param dir 
 * @return int 
 */
int sys_closedir(DIR *dir) {
  // 使用该文件系统关闭该目录
  fs_protect(root_fs);
  int err = root_fs->op->closedir(root_fs, dir);
  fs_unprotect(root_fs);
  return err;

}
/**
 * @brief 用指令cmd对io进行控制
 * 
 * @param fd 
 * @param cmd 
 * @param arg0 
 * @param arg1 
 * @return int 
 */
int sys_ioctl(int fd, int cmd, int arg0, int arg1) {
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

  fs_t *fs = file->fs;
  fs_protect(fs);
  int err = fs->op->ioctl(file, cmd, arg0, arg1);
  fs_unprotect(fs);

  return err;
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
    case FS_FAT16:
      return &fatfs_op;
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
  log_printf("mount file system, name: %s, dev: %x\n", mount_point, dev_major);

  // 1.检查当前文件系统是否已被挂载
  list_node_t *curr = list_get_first(&mounted_list);
  while (curr) {
    fs_t *fs = list_node_parent(curr, fs_t, node);
    if (kernel_strncmp(fs->mount_point, mount_point, FS_MOUNT_POINT_SIZE) ==
        0) {
      log_printf("fs already mounted!\n");
      goto mount_failed;
    }

    curr = list_node_next(curr);
  }

  // 2.从空闲链表中取下一个待挂载的fs对象进行挂载
  list_node_t *free_node = list_remove_first(&free_list);
  if (!free_node) {
    log_printf("no free fs, mount failed!\n");
    goto mount_failed;
  }
  fs = list_node_parent(free_node, fs_t, node);
  kernel_memset(fs, 0, sizeof(fs_t));
  kernel_strncpy(fs->mount_point, mount_point, FS_MOUNT_POINT_SIZE);

  // 3.获取该fs对象的操作函数表并交给该对象
  fs_op_t *op = get_fs_op(type, dev_major);
  if (!op) {
    log_printf("unsupported fs type: %du\n", type);
    goto mount_failed;
  }
  fs->op = op;

  // 4.挂载该文件系统类型下具体的设备
  if (op->mount(fs, dev_major, dev_minor) < 0) {
    log_printf("mount fs %s failed!\n", mount_point);
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

  disk_init();

  fs_t *fs = mount(FS_DEVFS, "/dev", 0, 0);
  ASSERT(fs != (fs_t *)0);

  root_fs = mount(FS_FAT16, "/home", ROOT_DEV);
  ASSERT(root_fs != (fs_t *)0);
}


