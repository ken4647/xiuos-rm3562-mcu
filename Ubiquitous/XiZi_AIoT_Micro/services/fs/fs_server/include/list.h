/*
 * Copyright (c) 2020 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#pragma once

struct double_list_node {
    struct double_list_node* next;
    struct double_list_node* prev;
};

#define CONTAINER_OF(item, type, member) \
    ((type*)((char*)(item) - (unsigned long)(&((type*)0)->member)))

#define IS_DOUBLE_LIST_EMPTY(head) \
    ((head)->next == (head))

/*********************************************
 *
 * Double linked list operation functions
 *
 *********************************************/
#define DOUBLE_LIST_ENTRY(item, type, member) \
    CONTAINER_OF(item, type, member)

#define DOUBLE_LIST_FOR_EACH_ENTRY(item, head, member)                      \
    for (item = DOUBLE_LIST_ENTRY((head)->next, __typeof__(*item), member); \
         &item->member != (head);                                           \
         item = DOUBLE_LIST_ENTRY(item->member.next, __typeof__(*item), member))

#define DOUBLE_LIST_FOR_EACH_ENTRY_REVERSE(item, head, member)              \
    for (item = DOUBLE_LIST_ENTRY((head)->prev, __typeof__(*item), member); \
         &item->member != (head);                                           \
         item = DOUBLE_LIST_ENTRY(item->member.prev, __typeof__(*item), member))

__attribute__((always_inline)) static void inline doubleListNodeInit(struct double_list_node* list)
{
    list->next = list;
    list->prev = list;
}

__attribute__((always_inline)) static void inline _double_list_add(struct double_list_node* new_node, struct double_list_node* prev, struct double_list_node* next)
{
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

__attribute__((always_inline)) static void inline _double_list_del(struct double_list_node* prev, struct double_list_node* next)
{
    next->prev = prev;
    prev->next = next;
}

__attribute__((always_inline)) static void inline doubleListAddOnHead(struct double_list_node* new_node, struct double_list_node* head)
{
    _double_list_add(new_node, head, head->next);
}

__attribute__((always_inline)) static void inline doubleListAddOnBack(struct double_list_node* new_node, struct double_list_node* head)
{
    _double_list_add(new_node, head->prev, head);
}

__attribute__((always_inline)) static void inline doubleListDel(struct double_list_node* entry)
{
    _double_list_del(entry->prev, entry->next);
    entry->next = entry;
    entry->prev = entry;
}
