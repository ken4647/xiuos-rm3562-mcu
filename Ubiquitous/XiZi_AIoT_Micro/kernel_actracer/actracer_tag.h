#pragma once
#include "list.h"

typedef enum {
    TRACER_INVALID = 0,
    TRACER_OWNER,
    TRACER_HARDKERNEL_AC_RESOURCE,
    TRACER_TASK_DESCRIPTOR_AC_RESOURCE,
    TRACER_SERVER_IDENTITY_AC_RESOURCE,
    TRACER_MEM_SIGNATURE,
    TRACER_SYSOBJECT,
} tracemeta_ac_type;

typedef struct TracerNode {
    tracemeta_ac_type type;
    char* name;
    void* p_resource;
    struct TracerNode* parent;
    struct double_list_node list_node;
    struct double_list_node children_guard;
} TracerNode;

/// @brief tag for other module to reference trace meta
typedef struct TraceTag {
    TracerNode* meta;
} TraceTag;