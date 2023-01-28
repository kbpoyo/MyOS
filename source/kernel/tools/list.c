/**
 * @file list.c
 * @author kbpoyo (kbpoyo.com)
 * @brief  定义链表数据结构
 * @version 0.1
 * @date 2023-01-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tools/list.h"

void list_insert_first(list_t *list, list_node_t *node) {
    ASSERT(list != (list_t *)0 && node != (list_node_t*)0);

    node->next = list->first;
    node->pre = (list_node_t*)0;

    if (list_is_empty(list)) {
        list->first = list->last = node;
    } else {
        list->first->pre = node;
        list->first = node;
    }

    list->size++;
}

void list_insert_last(list_t *list, list_node_t *node) {
    ASSERT(list != (list_t *)0 && node != (list_node_t*)0);

    node->pre = list->last;
    node->next = (list_node_t*)0;


    if (list_is_empty(list)) {
        list->first = list->last = node;
    } else {
        list->last->next = node;
        list->last = node;
    }

    list->size++;

}

list_node_t* list_remove_first(list_t *list){
    ASSERT(list != (list_t *)0);
    if (list_is_empty(list)) {
        return (list_node_t*)0;
    }

    list_node_t *ret = list->first;

    if (list_get_size(list) == 1) {
        list->first = list->last = (list_node_t*)0;
    } else {
        list->first = list->first->next;
        list->first->pre = (list_node_t*)0;
    }

    list->size--;

    ret->pre = ret->next = (list_node_t*)0;
    return ret;
 }

list_node_t* list_remove_last(list_t *list) {
    ASSERT(list != (list_t *)0);
     if (list_is_empty(list)) {
        return (list_node_t*)0;
    }

    list_node_t *ret = list->last;

    if (list_get_size(list) == 1) {
        list->first = list->last = (list_node_t*)0;
    } else {
        list->last = list->last->pre;
        list->last->next = (list_node_t*)0;
    }

    list->size--;

    ret->pre = ret->next = (list_node_t*)0;
    return ret;
}

list_node_t* list_remove(list_t *list, list_node_t *node) {
    ASSERT(list != (list_t *)0 && node != (list_node_t*)0);

    if (list_is_empty(list)) {
        return (list_node_t*)0;
    }

    if (node == list->first) list->first = node->next;
    if (node == list->last) list->last = node->pre;

    if (node->pre) node->pre->next = node->next;
    if (node->next) node->next->pre = node->pre;

    node->pre = node->next = (list_node_t*)0;
    list->size--;

    return node;
}