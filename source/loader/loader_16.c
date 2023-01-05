/**
 * @file loader_16.c
 * @author kbpoyo (kbpoyo.com)
 * @brief   loader程序将操作系统从磁盘载入到内存中，即从实模式到保护模式，即从16位模式到32位模式，该程序为loader运行在实模式下的部分
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

__asm__(".code16gcc"); //生成16位模式下的程序代码

#include "loader.h"

//实例化记录系统信息的结构体
_boot_info_t_ boot_info;

/**
 * @brief  
 * 在显示器上打印字符串msg 
 * @param msg 
 */
static void show_msg(const char* msg) {
     char c;
     while ((c = *(msg++)) != '\0') {          
          //asm()会对内联汇编进行优化，可能会导致不确定的结果，所以使用__asm__ __volatile__()
          __asm__ __volatile__(
               "mov $0xe, %%ah;"
               " mov %[rgs], %%al;"
               "int $0x10"::[rgs]"r"(c) //"r" 将rgs映射到任意寄存器, 
                                        //且有了输入参数后，真正的寄存器要用%%前缀进行访问
          );
     }
     
}


/**
 * @brief  
 * 检测系统当前可用的内存块的地址和大小
 */
static void detect_memory(void) {
     show_msg("try to detect memory:\r\n");

     SMAP_entry_t smap_entry;//记录每一次探测的结果
     
     boot_info.ram_region_count = 0; //将有效内存卡数量初始化为0

     //传入参数
     SMAP_entry_t *entry = &smap_entry; //记录的信息将回填到结构体中

     //传出参数
      uint32_t signature = 0, bytes =0, contID = 0; //分别为ax, cx, bx的传出参数

     //逐个检测内存块，有效则装入数组中
     for (int i = 0; i < BOOT_RAM_REGION_MAX; ++i) {

          //调用内联汇编进行一次内存块探测
          __asm__ __volatile__ ("int  $0x15" 
				: "=a"(signature), "=c"(bytes), "=b"(contID) //传出参数
				: "a"(0xE820), "b"(contID), "c"(24), "d"(0x534D4150), "D"(entry)); //传入参数

          //判断所探测的内存块是否有效
          if (signature != 0x534d4150) { //无效直接退出
               show_msg("failed\r\n");
               return;
          }

          if (bytes > 20 && (entry->ACPI & 0x0001) == 0) continue; //ACPI位为0，则内存块无效应当忽略	
		
          if (entry->Type == 1) { //Type = 1 当前内存块有效
               boot_info.ram_region_cfg[boot_info.ram_region_count].start = entry->BaseL;  //由于内存寻址空间较小，读取低32位即可
               boot_info.ram_region_cfg[boot_info.ram_region_count].size = entry->LengthL;
               boot_info.ram_region_count++;
          }

          if (contID == 0) break; //contID为0则探测结束
     } 

     show_msg("detect success\r\n");

}


void loader_entry(void) {
     show_msg("..........loading.........\r\n");

     detect_memory(); //检测可用内存块
     
     for (;;){
     };
}

