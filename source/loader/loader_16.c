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

//生成16位模式下的程序代码
__asm__(".code16gcc"); 

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
               //由于内存寻址空间较小，读取低32位即可
               boot_info.ram_region_cfg[boot_info.ram_region_count].start = entry->BaseL;  
               boot_info.ram_region_cfg[boot_info.ram_region_count].size = entry->LengthL;
               boot_info.ram_region_count++;
          }

          if (contID == 0) break; //contID为0则探测结束
     } 

     show_msg("detect success\r\n");

}


//全局描述符表GDT,里面每一个段描述都是八个字节大小
uint16_t gdt_table[][4] = {
     {0, 0, 0, 0}, //第0个段描述符无效，防止未初始化选择子时选中该段描述符
     {0xffff, 0x0000, 0x9a00, 0x00cf},
     {0xffff, 0x0000, 0x9200, 0x00cf},
};


/**
 * @brief  进入保护模式
 * 
 */
static void enter_protect_mode(void){
     //1.关闭中断，设置eflags对应的位
     cli();

     //2.以此打开A20Gate, 读取92端口，并将其第二位设置为1
     uint8_t v = inb(0x92);
     outb(0x92, v | 0x2);

     //3.加载全局描述符
     lgdt((uint32_t)gdt_table, sizeof(gdt_table));

     //4.开启保护模式的使能位，设置cr0寄存器的第0位PE设置为1
     uint32_t cr0 = read_cr0();
     write_cr0(cr0 | 0x1);

     //5.远跳转到32位的loader程序，并清空原来的16位指令流水线
     far_jump(8, (uint32_t)protect_mode_entry);
}

void loader_entry(void) {
     show_msg("..........loading.........\r\n");

     detect_memory();         //检测可用内存块
     enter_protect_mode();    //进入保护模式
     
     for (;;){
     };
}

