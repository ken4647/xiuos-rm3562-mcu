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
/**
 * @file actracer.c
 * @brief tracer implementation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */
/*************************************************
File name: actracer.c
Description: tracer implementation
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stddef.h>
#include <string.h>

#include "actracer.h"
#include "assert.h"

static struct SysTracer sys_tracer;
static char root_name[TRACER_NODE_NAME_LEN] = "ROOT\0";

static void tracer_init_node(TracerNode* node, char* name, tracemeta_ac_type type, void* p_resource)
{
    node->type = type;
    node->parent = NULL;
    if (name != NULL) {
        char* p_name = (char*)slab_alloc(&sys_tracer.node_name_allocator);
        if (!p_name) {
            p_name = "BAD_NAME(NOMEM)";
        } else {
            strcpy(p_name, name);
            p_name[TRACER_NODE_NAME_LEN - 1] = '\0';
            node->name = p_name;
        }
    }
    doubleListNodeInit(&node->children_guard);
    node->p_resource = p_resource;
    doubleListNodeInit(&node->list_node);
}

void sys_tracer_init()
{
    // set sys_tracer resource identity
    tracer_init_node(&sys_tracer.root_node, NULL, TRACER_OWNER, NULL);
    sys_tracer.root_node.name = root_name;
    sys_tracer.sys_tracer_tag.meta = &sys_tracer.root_node;

    // init memory allocator
    slab_init(&sys_tracer.node_allocator, sizeof(TracerNode), "TracerNodeAllocator");
    slab_init(&sys_tracer.node_name_allocator, sizeof(char[TRACER_NODE_NAME_LEN]), "TracerNodeNameAllocator");
}

static char* parse_path(char* path, char* const name)
{
    // skip extra '/'
    while (*path == '/') {
        path++;
    }
    if (*path == '\0') {
        return NULL;
    }

    // start of current name
    char* cur_start = path;
    while (*path != '/' && *path != '\0') {
        path++;
    }

    // handle current name
    int len = path - cur_start;
    if (len >= TRACER_NODE_NAME_LEN) {
        strncpy(name, cur_start, TRACER_NODE_NAME_LEN - 1);
        name[TRACER_NODE_NAME_LEN - 1] = '\0';
    } else {
        strncpy(name, cur_start, len);
        name[len] = '\0';
    }

    return path;
}

static TracerNode* tracer_find_node_onestep(TracerNode* const owner, const char* const name)
{
    TracerNode* iter = NULL;
    assert(owner->type == TRACER_OWNER);
    DOUBLE_LIST_FOR_EACH_ENTRY(iter, &owner->children_guard, list_node)
    {
        if (iter->name == NULL) {
            continue;
        }

        if (strcmp(name, iter->name) == 0) {
            return iter;
        }
    }

    return NULL;
}

TraceTag* const RequireRootTag()
{
    return &sys_tracer.sys_tracer_tag;
}

bool AchieveResourceTag(TraceTag* target, TraceTag* owner, char* name)
{
    static char name_buffer[TRACER_NODE_NAME_LEN];

    TracerNode* inner_node = owner->meta;
    assert(inner_node != NULL && inner_node->type == TRACER_OWNER);
    while ((name = parse_path(name, name_buffer)) != NULL) {
        if ((inner_node = tracer_find_node_onestep(inner_node, name_buffer)) == NULL) {
            DEBUG("Tracer: No such object, owner: %s, child: %s\n", //
                owner->meta->name == NULL ? "NULL" : owner->meta->name, name == NULL ? "NULL" : name_buffer);
            return false;
        }
    }

    target->meta = inner_node;
    return true;
}

void* AchieveResource(TraceTag* tag)
{
    assert(tag != NULL);
    if (tag->meta == NULL || tag->meta->type == TRACER_OWNER) {
        return NULL;
    }

    return tag->meta->p_resource;
}

bool CreateResourceTag(TraceTag* new_tag, TraceTag* owner, char* name, tracemeta_ac_type type, void* p_resource)
{
    assert(owner != NULL);
    if (owner->meta == NULL) {
        ERROR("Tracer: Empty owner, node name: %s\n", name);
        return false;
    }
    // assert(owner->meta->type == TRACER_OWNER);
    if (type == TRACER_SERVER_IDENTITY_AC_RESOURCE && //
        tracer_find_node_onestep(owner->meta, name) != NULL) {
        return false;
    }

    TracerNode* new_node = (TracerNode*)slab_alloc(&sys_tracer.node_allocator);
    if (new_node == NULL) {
        ERROR("Tracer: No memory for new node\n");
        return false;
    }
    tracer_init_node(new_node, name, type, p_resource);

    // new node add to owner's children list
    doubleListAddOnHead(&new_node->list_node, &owner->meta->children_guard);
    new_node->parent = owner->meta;

    if (new_tag != NULL) {
        new_tag->meta = new_node;
    }
    return true;
}

bool DeleteResource(TraceTag* target, TraceTag* owner)
{
    assert(target != NULL && owner != NULL);
    assert(owner->meta != NULL && owner->meta->type == TRACER_OWNER);
    if (target->meta == NULL) {
        ERROR("Tracer: Delete a empty resource, owner: %s\n", owner->meta->name);
        return false;
    }

    assert(target->meta->parent == owner->meta);
    doubleListDel(&target->meta->list_node);
    // delete name
    if (target->meta->name != NULL) {
        slab_free(&sys_tracer.node_name_allocator, target->meta->name);
    }

    // delete all children
    if (target->meta->type == TRACER_OWNER) {
        while (!IS_DOUBLE_LIST_EMPTY(&target->meta->children_guard)) {
            TraceTag tmp_node = {
                .meta = DOUBLE_LIST_ENTRY(target->meta->children_guard.next, TracerNode, list_node),
            };
            DeleteResource(&tmp_node, target);
        }
    }
    slab_free(&sys_tracer.node_allocator, target->meta);
    target->meta = NULL;
    return true;
}

#define debug_print_blanks(n)           \
    for (int __i = 0; __i < n; __i++) { \
        DEBUG_PRINTF("  ");             \
    }

void debug_list_tracetree_inner(TracerNode* cur_node, int nr_blanks)
{
    debug_print_blanks(nr_blanks);
    if (cur_node->name == NULL) {
        DEBUG_PRINTF("[ANON %d] ", cur_node->type);
    } else {
        DEBUG_PRINTF("[%s %d] ", cur_node->name, cur_node->type);
    }
    TracerNode* tmp = NULL;
    DOUBLE_LIST_FOR_EACH_ENTRY(tmp, &cur_node->children_guard, list_node)
    {
        if (tmp->name != NULL) {
            DEBUG_PRINTF("%s ", tmp->name);
        } else {
            DEBUG_PRINTF("ANON ");
        }
    }
    DEBUG_PRINTF("\n");
    DOUBLE_LIST_FOR_EACH_ENTRY(tmp, &cur_node->children_guard, list_node)
    {
        debug_list_tracetree_inner(tmp, nr_blanks + 1);
    }
}

void debug_list_tracetree()
{
    TracerNode* ref_root = RequireRootTag()->meta;
    debug_list_tracetree_inner(ref_root, 0);
}