/**
 * @file syscall.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 调用门处理函数，即各个系统函数的调用
 * @version 0.1
 * @date 2023-05-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "cpu/syscall.h"
#include "core/memory.h"
#include "core/task.h"
#include "tools/log.h"
#include "fs/fs.h"


/**
 * @brief 临时的格式化输出系统调用
 * 
 * @param fmt 
 * @param arg 
 */
void sys_print_msg(const char *fmt, int arg) {
    log_printf(fmt, arg);
}

static const sys_handler_t sys_table[] = {
    [SYS_sleep] = (sys_handler_t)sys_sleep,
    [SYS_getpid] = (sys_handler_t)sys_getpid,
    [SYS_fork] = (sys_handler_t)sys_fork,
    [SYS_execve] = (sys_handler_t)sys_execve,
    [SYS_yield] = (sys_handler_t)sys_yield,
    [SYS_printmsg] = (sys_handler_t)sys_print_msg,
    [SYS_open] = (sys_handler_t)sys_open,
    [SYS_read] = (sys_handler_t)sys_read,
    [SYS_write] = (sys_handler_t)sys_write,
    [SYS_close] = (sys_handler_t)sys_close,
    [SYS_lseek] = (sys_handler_t)sys_lseek,
    [SYS_isatty] = (sys_handler_t)sys_isatty,
    [SYS_fstat] = (sys_handler_t)sys_fstat,
    [SYS_sbrk] = (sys_handler_t)sys_sbrk,
    [SYS_dup] = (sys_handler_t)sys_dup,
    [SYS_exit] = (sys_handler_t)sys_exit,
    [SYS_wait] = (sys_handler_t)sys_wait,
    [SYS_opendir] = (sys_handler_t)sys_opendir,
    [SYS_readdir] = (sys_handler_t)sys_readdir,
    [SYS_closedir] = (sys_handler_t)sys_closedir,
    [SYS_ioctl] = (sys_handler_t)sys_ioctl,
    [SYS_unlink] = (sys_handler_t)sys_unlink,

};

/**
 * @brief 门调用处理函数，通过定义的系统调用id，将该调用分发到正确的系统调用上
 * 
 * @param frame 
 */
void do_handler_syscall(syscall_frame_t* frame) {
    if (frame->function_id < sizeof(sys_table) / sizeof(sys_table[0])) {    //当前系统调用存在
        sys_handler_t handler = sys_table[frame->function_id];
        if (handler) {
            //直接将4个参数全部传入即可，
            //因为是按从右到左的顺序将参数压栈，所以原始的参数只要是从arg0开始赋值的即可，
            //多余的参数在高地址处，不影响handler对应的真正的系统调用
            int ret = handler(frame->arg0, frame->arg1, frame->arg2, frame->arg3);
            //正常函数返回后会将返回值先存放到eax寄存器中，再eax中的值放入对应接收返回值的内存中
            //此处用eax先接收ret，在调用门返回后再从eax中取处该值
            frame->eax = ret;
            return;
        }
    }
    
    //打印系统调用失败的异常日志
    task_t *task = task_current();
    log_printf("task: %s, Unknown syscall_id: %d\n", task->name, frame->function_id);
    frame->eax = -1;

}