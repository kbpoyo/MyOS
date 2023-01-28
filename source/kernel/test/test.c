/**
 * @file test.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  编写内核测试代码
 * @version 0.1
 * @date 2023-01-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/list.h"
#include "tools/log.h"
#include "common/types.h"

void list_test(void) {
    list_t list;
    list_node_t nodes[5];

    list_init(&list);
    log_printf("list: first=0x%x, last=0x%x, size=%d", 
    list_get_first(&list), list_get_last(&list), list_get_size(&list));

    //测试list_insert_first
    for (int i = 0; i < 5; ++i) {
        list_node_t *node = nodes + i;

        log_printf("insert first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_first(&list, node);
    }

    log_printf("list: first=0x%x, last=0x%x, size=%d", 
    list_get_first(&list), list_get_last(&list), list_get_size(&list));

   
    //测试list_remove_first
    for (int i = 0; i < 5; ++i) {
        list_node_t *node = list_remove_first(&list);
        log_printf("remove first: %d, 0x%x", i, (uint32_t)node);
    }

    log_printf("list: first=0x%x, last=0x%x, size=%d", 
    list_get_first(&list), list_get_last(&list), list_get_size(&list));
    

    //测试list_insert_last
    for (int i = 0; i < 5; ++i) {
        list_node_t *node = nodes + i;

        log_printf("insert last to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_last(&list, node);
    }

    log_printf("list: first=0x%x, last=0x%x, size=%d", 
    list_get_first(&list), list_get_last(&list), list_get_size(&list));

    //测试list_remove_last
    for (int i = 0; i < 5; ++i) {
        list_node_t *node = list_remove_last(&list);
        log_printf("remove last: %d, 0x%x", i, (uint32_t)node);
    }

    log_printf("list: first=0x%x, last=0x%x, size=%d", 
    list_get_first(&list), list_get_last(&list), list_get_size(&list));
    
    struct type_t {
        int i;
        list_node_t node;
    }v = {0x123456};

    list_node_t *v_node = &v.node;
    struct type_t *p = list_node_parent(v_node, struct type_t, node);  

    log_printf("v addr = 0x%x", (uint32_t)&v);
    log_printf("p addr = 0x%x", (uint32_t)p);  

 }
