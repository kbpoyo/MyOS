/**
 * @file lib_syscall.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 系统调用
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include    "common/types.h"
#include    "cpu/syscall.h"
#include    "os_cfg.h"
#include    "lib_syscall.h"


int sys_call(syscall_args_t *args) {
    // 传入远跳转需要的参数, 即cs = selector, eip = offset 
    //为调用门选择子赋予0特权级,调用门的 cpl 和 RPL <= 系统调用段描述符的DPL
    //门描述符中目标选择子的 CPL <= 门描述符的 DPL
    uint32_t addr[] = {0, SYSCALL_SELECTOR | 0};  

    //lcalll指令是一个汇编伪指令，它用于调用远子程序，
    //它会将当前的CS和IP压入栈中，并跳转到指定的段选择子和偏移量⁴。
    //lcalll指令可以实现特权级的转换，可以触发调用门机制。
    //将参数压入用户栈中，再跳转到调用门，cpu会将用户的ss和esp寄存器压入栈中，并将用户栈中的参数拷贝到内核栈中
    //再压入 cs 和 eip 寄存器

    int ret;

     __asm__ __volatile__(
        "push %[arg3]\n\t"
        "push %[arg2]\n\t"
        "push %[arg1]\n\t"
        "push %[arg0]\n\t"
        "push %[id]\n\t"
        "lcalll *(%[addr])\n\t"
        :"=a"(ret)
        :[arg3]"r"(args->arg3), 
         [arg2]"r"(args->arg2), 
         [arg1]"r"(args->arg1),
         [arg0]"r"(args->arg0), 
         [id]"r"(args->id), 
         [addr]"r"(addr)
    );

    return ret;
}




/**
 * @brief 以ms为单位进行延时
 * 
 * @param ms 
 */
void msleep(int ms) {
    if (ms <= 0) return;

    syscall_args_t args;
    args.id = SYS_sleep;
    args.arg0 = ms;

    sys_call(&args);

}

/**
 * @brief 获取用户进程id
 * 
 * @return int 
 */
int getpid(void) {
    syscall_args_t args;
    args.id = SYS_getpid;

    return sys_call(&args);
}

void print_msg(const char *fmt, int arg) {
    syscall_args_t args;
    args.id = SYS_printmsg;
    args.arg0 = (int)fmt;
    args.arg1 = arg;

    sys_call(&args);
}

int fork (void) {
    syscall_args_t args;
    args.id = SYS_fork;

    return sys_call(&args);
}


/**
 * @brief 加载执行外部程序
 * 
 * @param name 外部程序名
 * @param argv 外部程序的参数，字符串常量指针，即字符串数组，数组中的char*值为常量
 * @param env  所加载程序的环境变量
 * @return int 
 */
int execve(const char *name, char * const * argv, char * const * env) {
    syscall_args_t args;
    args.id = SYS_execve;
    args.arg0 = (int)name;
    args.arg1 = (int)argv;
    args.arg2 = (int)env;


    return sys_call(&args);
}

/**
 * @brief 进程主动放弃cpu
 * 
 * @return int 
 */
int yield (void) {
    syscall_args_t args;
    args.id = SYS_yield;

    return sys_call(&args);
}

/**
 * @brief 打开文件
 * 
 * @param name 
 * @param flags 
 * @param ... 
 * @return int 
 */
int open(const char *name, int flags, ...) {
    syscall_args_t args;
    args.id = SYS_open;
    args.arg0 = (int)name;
    args.arg1 = flags;

    return sys_call(&args);
}

/**
 * @brief 读取文件
 * 
 * @param file 
 * @param ptr 
 * @param len 
 * @return int 
 */
int read(int file, char *ptr, int len) {
    syscall_args_t args;
    args.id = SYS_read;
    args.arg0 = file;
    args.arg1 = (int)ptr;
    args.arg2 = len;

    return sys_call(&args);
}
/**
 * @brief 写文件
 * 
 * @param file 
 * @param ptr 
 * @param len 
 * @return int 
 */
int write(int file, char *ptr, int len) {
    syscall_args_t args;
    args.id = SYS_write;
    args.arg0 = file;
    args.arg1 = (int)ptr;
    args.arg2 = len;

    return sys_call(&args);
}

/**
 * @brief 关闭文件描述符
 * 
 * @param file 
 * @return int 
 */
int close(int file) {
    syscall_args_t args;
    args.id = SYS_close;
    args.arg0 = file;

    return sys_call(&args);
}

/**
 * @brief 使文件file位置从dir位置偏移offset
 * 
 * @param file 
 * @param offset 
 * @param dir 
 * @return int 
 */
int lseek(int file, int offset, int dir) {
    syscall_args_t args;
    args.id = SYS_lseek;
    args.arg0 = file;
    args.arg1 = offset;
    args.arg2 = dir;

    return sys_call(&args);
}

/**
 * @brief 
 * 
 * @param file 
 * @return int 
 */
int isatty(int file) {
    syscall_args_t args;
    args.id = SYS_isatty;
    args.arg0 = file;

    return sys_call(&args);
}
/**
 * @brief 
 * 
 * @param file 
 * @param st 
 * @return int 
 */
int fstat(int file, struct stat *st) {
    syscall_args_t args;
    args.id = SYS_fstat;
    args.arg0 = file;
    args.arg1 = (int)st;

    return sys_call(&args);
}

/**
 * @brief 
 * 
 * @param incr 
 * @return void* 
 */
char *sbrk(ptrdiff_t incr) {
    syscall_args_t args;
    args.id = SYS_sbrk;
    args.arg0 = (int)incr;

    return (char*)sys_call(&args);
}