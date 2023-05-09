/**
 * @file gate.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 定义门结构的方法
 * @version 0.1
 * @date 2023-05-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "cpu/gate.h"

/**
 * @brief  设置门描述符, 调用门，中断门，任务门，陷阱门
 *
 * @param desc 需要被设置的中断门描述符结构
 * @param selector 门描述符对应的处理程序所在的段的选择子
 * @param offset 门描述符对应的处理程序相对于所在段的偏移量
 * @param attr 门描述符的属性
 */
void gate_desc_set(gate_desc_t *desc, const uint16_t selector, const uint32_t offset,
                          const uint16_t attr) {
  // 1.初始化偏移量
  desc->offset15_0 = offset & 0xffff;
  desc->offset31_16 = (offset >> 16) & 0xffff;

  // 2.初始化选择子
  desc->selector = selector;

  // 3.初始化属性位
  desc->attr = attr;
}