#pragma once

#include <stddef.h>
#include <stdint.h>

#include "actracer.h"

#define RBTTREE_INSERT_SECC 0
#define RBTTREE_INSERT_FAILED -1
#define RBTTREE_INSERT_EXISTED -2

#define RBTTREE_DELETE_SUCC 0
#define RBTTREE_DELETE_FAILED -1

// CLRS
// Insertion and Deletion in a Red Black Tree
enum rbt_type {
    RED,
    BLACK
};

typedef struct RbtNode {
    uintptr_t key;
    void* data;
    struct RbtNode* left;
    struct RbtNode* right;
    struct RbtNode* parent;
    enum rbt_type color;
} RbtNode;

typedef struct RbtTree {
    RbtNode* root;
    int nr_ele;
} RbtTree;

// return if the traverse needs to continue
typedef bool(rbt_traverse_fn)(RbtNode* node, void* data);

void rbtree_init(RbtTree* tree);
int rbt_insert(RbtTree* tree, uintptr_t key, void* data);
RbtNode* rbt_search(RbtTree* tree, uintptr_t key);
int rbt_delete(RbtTree* tree, uintptr_t key);
void rbt_traverse(RbtTree* tree, rbt_traverse_fn fn, void* data);

void module_rbt_factory_init(TraceTag* _softkernel_tag);

static inline bool rbt_is_empty(RbtTree* tree)
{
    return tree->nr_ele == 0;
}
