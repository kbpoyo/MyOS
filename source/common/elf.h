/**
 * @file elf.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  解析elf文件头，elf header
 * @version 0.1
 * @date 2023-01-08
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ELF_H
#define ELF_H

#include "types.h"

// ELF相关数据类型
typedef uint16_t Elf32_Half;    //2个字节类型
typedef uint32_t Elf32_Word;    //4个字节类型
typedef uint32_t Elf32_Addr;    //程序运行地址
typedef uint32_t Elf32_Off;     //文件偏移量

// ELF Header
#define EI_NIDENT       16      //e_ident数组大小固定为16，开头4字节是elf的魔数，相当于elf文件的标识符
                                //4位：标识文件标类型
                                //5位：指定字节序
                                //6位：elf头的版本信息默认为 1
                                //7~15位：保留位，均初始化为 0

#define ELF_MAGIC       0x7F    //e_ident数组的第一个字节,后序3字节为'E','L','F'

#define ET_EXEC         2       // 可执行文件,ELF头的类型：e_type
#define EM_386          3       // 80386处理器，体系结构类型：e_machine

//内存按1字节对齐，防止结构体元素之间不连续
#pragma pack(1)
//书p216, ELF header 结构体
typedef struct {
    uint8_t e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
}Elf32_Ehdr;


#define PT_LOAD   1 // 可加载类型,程序头，即段头的类型：p_type

//书p217 program header 结构体
typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;


//将内存对齐改回默认值
#pragma pack()
#endif