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

/**
 * @brief 执行调用main之前的初始化工作
 *
 * @param argc
 * @param argv
 */
void cstart(int argc, char **argv) {
    main(argc, argv);
}
