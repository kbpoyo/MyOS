/**
 * @file list.h
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义链表数据结构
 * @version 0.1
 * @date 2023-01-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LIST_H
#define LIST_H

#include "tools/assert.h"
//链表节点
typedef struct _list_node_t {
    struct _list_node_t * pre;
    struct _list_node_t * next; 

}list_node_t;


//双向链表
typedef struct  _list_t {
    list_node_t * first;
    list_node_t * last;
    int size;
}list_t;

static inline void list_node_init(list_node_t *node) {
    ASSERT(node != (list_node_t*)0);
    node->pre = node->next = (list_node_t*)0;
}

static inline list_node_t * list_node_pre(list_node_t *node) {
    ASSERT(node != (list_node_t*)0);

    return node->pre;
}

static inline list_node_t *list_node_next(list_node_t *node) {
    ASSERT(node != (list_node_t*)0);

    return node->next;
}

static inline void list_init(list_t *list) {
    ASSERT(list != (list_t*)0);

    list->first = list->last = (list_node_t*)0;
    list->size = 0;
}

static inline int list_is_empty(list_t *list) {
    ASSERT(list != (list_t*)0);

    return list->size == 0;
}

static inline int list_get_size(list_t *list) {
    ASSERT(list != (list_t*)0);

    return list->size;
}

static inline list_node_t *list_get_first(list_t *list) {
    ASSERT(list != (list_t*)0);

    return list->first;
}

static inline list_node_t *list_get_last(list_t *list) {
    ASSERT(list != (list_t*)0);

    return list->last;
}

void list_insert_first(list_t *list, list_node_t *node);

void list_insert_last(list_t *list, list_node_t *node);

list_node_t* list_remove_first(list_t *list);

list_node_t* list_remove_last(list_t *list);

list_node_t* list_remove(list_t *list, list_node_t *node);

/**
 * @brief  计算node类型的变量在所属结构体中的偏移量
 * 
 * @param parnet_type 所属结构体的类型
 * @param node_name node类型在所属结构体中的变量名
 */
#define offset_in_parent(parent_type, node_name) \
    ((uint32_t)&(((parent_type *)0)->node_name))

/**
 * @brief  计算node所属结构体的地址
 * 
 * @param node 当前node类型的变量名
 * @param parnet_type 所属结构体的类型
 * @param node_name node类型在所属结构体中的变量名
 */
#define parent_addr(node, parent_type, node_name) \
    ((uint32_t)node - offset_in_parent(parent_type, node_name))
/**
 * @brief  得到node所属结构体的指针
 * 
 * @param node 当前node类型的变量名
 * @param parnet_type 所属结构体的类型
 * @param node_name node类型在所属结构体中的变量名
 */
#define list_node_parent(node, parent_type, node_name) \
    ((parent_type *)(node ? parent_addr(node, parent_type, node_name) : 0))

#endif