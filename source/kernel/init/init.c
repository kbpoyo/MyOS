/**
 * @file init.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  对内核进行初始化
 * @version 0.1
 * @date 2023-01-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "init.h"
#include "common/boot_info.h"
#include "common/cpu_instr.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "tools/log.h"
#include "tools/assert.h"
#include "dev/time.h"
#include "core/task.h"
#include "os_cfg.h"
#include "tools/list.h"
#include "test/test.h"
#include "tools/klib.h"
#include  "ipc/sem.h"
#include "core/memory.h"

/**
 * @brief  对内核进行初始化操作
 * 
 */
void kernel_init(boot_info_t* boot_info) {
    //1.判断是否检测到可用内存
    ASSERT(boot_info->ram_region_count != 0);
    
    //2.初始化日志程序,便于后期调用
    log_init();

    //3.初始化内存管理
    memory_init(boot_info);  

    //4.初始化并重新加载GDT表
    gdt_init();

    //5.初始化并加载中断描述符表IDT
    idt_init();

    //6.初始化定时器的中断处理
    time_init();

    //7.初始化任务管理器
    task_manager_init();
    //初始化完成后将在汇编里重新加载内核代码段与数据段的选择子，并为内核程序分配栈空间

}

static task_t task_test_task_1;
static task_t task_test_task_2;
static uint32_t test_task_1_stack[1024]; 
static uint32_t test_task_2_stack[1024]; 
static sem_t sem;


void test_task_1(void) {

    int count = 0;
    for (;;) {
        // sem_wait(&sem);
        log_printf("task_1: %d", count++);
        // sem_notify(&sem);
        // sys_sleep(1000);
     }
}

void test_task_2(void) {

    int count = 0;
    for (;;) {
        // sem_wait(&sem);
        log_printf("task_2: %d", count++);
        // sem_notify(&sem);
        // sys_sleep(1000);
     }
}



void init_main(void) {


    
    log_printf("Kernel is running......");
    log_printf("Name: %s, Version: %s", "KbOS", OS_VERSION);

    //当前任务作为任务管理器启用时的第一个任务
    task_first_init();
    task_init(&task_test_task_1, "test_task_1", (uint32_t)test_task_1, (uint32_t)&test_task_1_stack[1024]);
    task_init(&task_test_task_2, "test_task_2", (uint32_t)test_task_2, (uint32_t)&test_task_2_stack[1024]);


    // sem_init(&sem, 0);
    sti();

    int count = 0;
    for (;;) {
        log_printf("first: %d", count++);
        // sem_notify(&sem);
        // sys_sleep(1000);
    }
}