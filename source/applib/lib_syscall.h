/**
 * @file lib_syscall.h
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 供外部应用程序编码时使用的库文件，对系统调用进行封装
 * @version 0.1
 * @date 2023-05-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "common/types.h"
#include "cpu/syscall.h"
#include "os_cfg.h"
#include <sys/stat.h>  

/**
 * @brief 系统调用的参数结构体
 * 
 */
typedef struct _syscall_args_t {
    int id; //真正的系统调用函数的id
    //系统调用的函数的4个参数
    int arg0;
    int arg1;
    int arg2;
    int arg3;
}syscall_args_t;

void print_msg(const char *fmt, int arg);

//进程相关系统调用
int getpid(void);
void msleep(int ms);
int fork (void);
int execve(const char *name, char * const * argv, char * const * env);
int yield (void);
int wait(int *status);
void _exit(int status);




//提供给newlib库的系统调用
//文件操作相关系统调用
int open(const char *name, int flags, ...);
int read(int file, char *ptr, int len);
int write(int file, char *ptr, int len);
int close(int file);
int lseek(int file, int offset, int dir);
int ioctl(int file, int cmd, int arg0, int arg1);

int isatty(int file);
int fstat(int file, struct stat *st);
char *sbrk(ptrdiff_t incr);

int dup(int file);


//文件目录项结构
typedef struct dirent {
    int index;
    int type;
    char name[14];
    int size;
}dirent;

//文件目录对象结构
typedef struct _DIR {
    int index;
    struct dirent dirent;
}DIR;

//目录操作的系统调用
DIR *opendir(const char *path);
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);


#endif