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

#include    "common/types.h"
#include    "cpu/syscall.h"
#include    "os_cfg.h"

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




static inline int sys_call(syscall_args_t *args) {
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
static inline void msleep(int ms) {
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
static inline int getpid(void) {
    syscall_args_t args;
    args.id = SYS_getpid;

    return sys_call(&args);
}

static inline void print_msg(const char *fmt, int arg) {
    syscall_args_t args;
    args.id = SYS_printmsg;
    args.arg0 = (int)fmt;
    args.arg1 = arg;

    sys_call(&args);
}

static inline int fork (void) {
    syscall_args_t args;
    args.id = SYS_fork;

    return sys_call(&args);
}


#endif