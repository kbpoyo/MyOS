/**
 * @file cstart.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief c运行时库，加载程序运行前的初始化工作的c语言部分
 * @version 0.1
 * @date 2023-05-12
 *
 * @copyright Copyright (c) 2023
 *
 */

int main(int argc, char **argv);
extern char __bss_start__[], __bss_end__[];

/**
 * @brief 执行调用main之前的初始化工作
 *
 * @param argc
 * @param argv
 */
void cstart(int argc, char **argv) {
    //对程序的bss段进行清零操作
    char *start = __bss_start__;
    while (start < __bss_end__) {
        *(start++);
    }
    
    main(argc, argv);
}
