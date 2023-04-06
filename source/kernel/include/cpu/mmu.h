/**
 * @file mmu.h
 * @author your name (you@domain.com)
 * @brief 声明定义虚拟内存管理的变量与方法
 * @version 0.1
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MMU_H
#define MMU_H

#include "common/types.h"
#include "common/cpu_instr.h"

//定义页目录项相关的宏
#define PDE_CNT 1024        //页目录项的个数,1024个即可，每个目录项4字节，刚好4kb即页目录表刚好占一个页
#define PDE_P   (1 << 0)    //第0位，present位
#define PDE_W   (1 << 1)    //第1位，write位
#define PDE_U   (1 << 2)    //第2位，user位

//定义页表项相关的宏
#define PTE_P   (1 << 0)    //第0位，present位
#define PTE_W   (1 << 1)    //第1位，
#define PTE_U   (1 << 2)    //第2位，user位




#pragma pack(1)

//csapp p578
//定义页目录项PDE结构的联合体
typedef union _pde_t {
    uint32_t v;
    struct {
        uint32_t present : 1;   //P位，1：子页表存在于内存中
        uint32_t write_enable : 1; //R/W位， 1：对于子页表中所有可访问页可读写
        uint32_t user_mode_acc : 1; //U/S位，1：用户以超级用户的权限访问
        uint32_t write_through : 1; //PWT位 子页表的直写或写回策略
        uint32_t cache_disable : 1; //PCD位，能否缓存子页表PTE到TLB中
        uint32_t accessed : 1;  //A位,引用位，标志子页表是否被访问
        uint32_t : 1;
        uint32_t ps : 1;    //PS位，标志页大小为4kb或4mb,只对第一层级的PTE有校
        uint32_t : 4;
        uint32_t phy_pt_addr : 20;  //高20位，页表的物理地址
    };
    
}pde_t;

//定义页表项PTE结构的联合体
typedef union _pte_t {
    uint32_t v;
    struct {
        uint32_t present : 1;   //P位，1：子页存在于内存中
        uint32_t write_enable : 1; //R/W位， 1：对于子页可读写
        uint32_t user_mode_acc : 1; //U/S位，1：用户以超级用户的权限访问
        uint32_t write_through : 1; //PWT位 子页的直写或写回策略
        uint32_t cache_disable : 1; //PCD位，能否缓存子页表PTE到TLB中
        uint32_t accessed : 1;  //A位,引用位，标志子页表是否被访问
        uint32_t dirty : 1;     //D位，修改位，标记该页是否被写
        uint32_t pat : 1;
        uint32_t gloable : 1;   //G位，全局位，标记该页在任务切换时是否从TLB缓存中驱逐出去
        uint32_t : 3;
        uint32_t phy_page_addr : 20;  //高20位，页的物理地址
    };
    
}pte_t;

#pragma pack()

/**
 * @brief 获取虚拟地址的高10位，及对应的页目录项在页目录表中的索引
 * 
 * @param vstart 
 * @return uint32_t 
 */
static inline uint32_t pde_index(uint32_t vstart) {
    return (vstart >> 22);
}

/**
 * @brief 获取虚拟地址的次10位，及对应的页表项在页表中的索引
 * 
 * @param vstart 
 * @return uint32_t 
 */
static inline uint32_t pte_index(uint32_t vstart) {
    return (vstart >> 12) & 0x3ff;
}

/**
 * @brief 获取页目录项中对应的页表的起始地址，及该页表第一个页表项的地址
 * 
 * @param pde 页目录项
 * @return uint32_t 返回的页表的地址
 */
static inline uint32_t pde_to_pt_addr(pde_t *pde) {
    //高20位为页表的物理地址的有效位，将其左移12位，及按4kb对齐后才是该页的物理地址
    return pde->phy_pt_addr << 12;
}


/**
 * @brief 获取页表项中对应的页的起始地址
 * 
 * @param pte 页表项
 * @return uint32_t 返回的页的地址
 */
static inline uint32_t pte_to_pg_addr(pte_t *pte) {
    //高20位为页的物理地址有效位，将其左移12位，及按4kb对齐后才是该页的物理地址
    return pte->phy_page_addr << 12;
}


/**
 * @brief 设置页目录表的起始地址，将起始地址写入CR3寄存器
 * 
 * @param paddr 页目录表的物理起始地址
 */
static inline void mmu_set_page_dir(uint32_t paddr) {

    //设置cr3寄存器的高20位为页目录表的地址，因为按4kb对齐，所以
    //页目录表的起始地址page_dir的高20位才为有效位，低12位为0，将cr3的低12位就设置为0
    write_cr3(paddr);
}

#endif
