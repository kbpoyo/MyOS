/**
 * @file gdt.c
 * @author kbpoyo (kbpoyo.com)
 * @brief   定义段描述的数据结构以及相关属性位的宏，以及全局描述符表
 * @version 0.1
 * @date 2023-01-09
 * @copyright Copyright (c) 2023
 * 
 */

#include "cpu/gdt.h"


/**
 * @brief  添加段描述到GDT中
 * 
 * @param selector 选择子，即实模式下的段基址，16位，0~1位为4种特权级，
 *                 2位标识是GDT还是LDT, 3~15位为所指向的段的下标
 *                 即2的13次方个段，也就是8192
 * @param base 该段在内存中的真实段基址
 * @param limit 该段的段界限，是一个偏移量，表示段中最大的偏移界限
 * @param attr 该段的属性位
 */
void segment_desc_set(uint16_t selector,  uint32_t base, uint32_t limit, uint16_t attr) {
    //1. 获取该段描述符在内存中的起始地址，selector >> 3 取出3~15位才是其在全局描述符中的下标
    segment_desc_t *desc = gdt_table + (selector >> 3);

    //2. 设置段基地址
    desc->base15_0 = (uint16_t)base;
    desc->base23_16 = (uint8_t)(base >> 16);
    desc->base31_24 = (uint8_t)(base >> 24);

    //3. 设置段界限
    if (limit > 0xfffff) { //limit大于了20位能表示的最大值
        //将limit的粒度设置为 4kb，即将属性位G置1
        desc->attr |= 0x8000;

        //将limit改成以 4kb 为单位
        limit /= (4 * 1024);

    }
    desc->limit15_0 = (uint16_t)limit;
    desc->attr |= ((uint16_t)(limit >> 8) & 0x0f00); //将limit高4位赋值到attr的8~11位(段界限的高4位)

    //4.设置属性位
    desc->attr |= attr & 0xf0ff;


}

/**
 * @brief  初始化GDT
 * 
 */
void gdt_init(void) {
    //TODO:暂时将所有段都初始化为0
    for (int i = 0; i < GDT_TABLE_SIZE; ++i) {
        segment_desc_set(i << 3, 0, 0, 0);
    }

    //使用平坦模型，即段基址为0, 段界限直接用最大值，界限粒度为4kb，即段大小为4GB
    //1.设置内核的数据段
    segment_desc_set(KERNEL_SELECTOR_DS, 0, 0xffffffff,
                    SEG_ATTR_P | SEG_ATTR_DPL_0 | SEG_ATTR_S_NORMAL | 
                    SEG_ATTR_TYPE_DATA | SEG_ATTR_TYPE_RW | SEG_ATTR_D_OR_B);

    //2.设置内核的代码段
    segment_desc_set(KERNEL_SELECTOR_CS, 0, 0xffffffff,
                    SEG_ATTR_P | SEG_ATTR_DPL_0 | SEG_ATTR_S_NORMAL |
                    SEG_ATTR_TYPE_CODE | SEG_ATTR_TYPE_RW | SEG_ATTR_D_OR_B);

    //3.加载新的GDT表
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

