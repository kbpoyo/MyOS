/**
 * @file loader.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LOADER_H
#define LOADER_H

#include "common/boot_info.h"
#include "common/types.h"
#include "common/cpu_instr.h"


//用于检测有效内存块的结构体
typedef struct SMAP_entry {
 
	uint32_t BaseL; // base address uint64_t, 即基础寻址空间为64位
	uint32_t BaseH;
	uint32_t LengthL; // length uint64_t
	uint32_t LengthH;
	uint32_t Type; // entry Type，值为1时表明为我们可用的RAM空间
	uint32_t ACPI; // extended, bit0=0时表明此条目应当被忽略
 
}__attribute__((packed)) SMAP_entry_t;

/**
 * @brief  进入保护模式
 * 
 */
void protect_mode_entry(void);

#endif