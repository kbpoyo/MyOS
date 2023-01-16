/**
 * @file assert.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义ASSERT宏，方便在debug版本下进行调试使用
 * @version 0.1
 * @date 2023-01-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/assert.h"
#include "common/cpu_instr.h"
#include "tools/log.h"


/**
 * @brief  打印发生异常的位置信息
 * 
 * @param file 异常所在文件绝对路径
 * @param line 异常所在行号
 * @param func 异常所在函数名 
 * @param reason 异常的原因
 */
void pannic(const char *file, int line, const char *func, const char *reason) {
    log_printf("assert faild! %s", reason);
    log_printf("file:\t%s\nline:\t%d\nfunc:\t%s", file, line, func);
    for (;;) {
        hlt();
    }

}