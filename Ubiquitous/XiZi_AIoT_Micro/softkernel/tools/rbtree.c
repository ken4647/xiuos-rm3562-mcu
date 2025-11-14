
#include <stddef.h>

#include "assert.h"
#include "rbtree.h"

struct RbtFactory {
    TraceTag tag;
    struct slab_allocator rbtnode_ele_allocator;
};

static struct RbtFactory rbt_factory;

void module_rbt_factory_init(TraceTag* _softkernel_tag)
{
    CreateResourceTag(&rbt_factory.tag, _softkernel_tag, "GlobalRbtFactory", TRACER_SYSOBJECT, &rbt_factory);
    slab_init(&rbt_factory.rbtnode_ele_allocator, sizeof(struct RbtNode), "RbtNodeAllocator");
}

void delete_case1(RbtTree* tree, RbtNode* node);
void delete_case2(RbtTree* tree, RbtNode* node);
void delete_case3(RbtTree* tree, RbtNode* node);
void delete_case4(RbtTree* tree, RbtNode* node);
void delete_case5(RbtTree* tree, RbtNode* node);
void delete_case6(RbtTree* tree, RbtNode* node);

static inline enum rbt_type get_color(RbtNode* node)
{
    if (node == NULL)
        return BLACK;
    else
        return node->color;
}

static inline void set_color(enum rbt_type color, RbtNode* node)
{
    assert(node != NULL);
    node->color = color;
}

static inline RbtNode* get_parent(RbtNode* node)
{
    assert(node != NULL);
    return node->parent;
}

static inline void set_parent(RbtNode* parent, RbtNode* node)
{
    assert(node != NULL);
    node->parent = parent;
}

static int is_root(RbtNode* node)
{
    assert(node != NULL);
    return (get_parent(node) == NULL);
}

static inline int is_black(RbtNode* node)
{
    assert(node != NULL);
    return (get_color(node) == BLACK);
}

static inline int is_red(RbtNode* node)
{
    assert(node != NULL);
    return (get_color(node) == RED);
}

RbtNode* sibling(RbtNode* node)
{
    assert(node != NULL);
    assert(node->parent != NULL); /* Root node has no sibling */
    if (node == node->parent->left)
        return node->parent->right;
    else
        return node->parent->left;
}
static inline RbtNode* get_min(RbtNode* node)
{
    assert(node != NULL);
    while (node->left) {
        node = node->left;
    }
    return node;
}

static inline RbtNode* get_max(RbtNode* node)
{
    assert(node != NULL);
    while (node->right) {
        node = node->right;
    }
    return node;
}

RbtNode* rbtree_min(RbtTree* tree)
{
    if (tree->root == NULL)
        return NULL;
    else {
        return get_min(tree->root);
    }
}

RbtNode* rbtree_max(RbtTree* tree)
{
    if (tree->root == NULL)
        return NULL;
    else {
        return get_max(tree->root);
    }
}

RbtNode* rbtree_prev(RbtNode* node)
{
    assert(node != NULL);
    if (node->left) {
        return get_max(node->left);
    } else {
        RbtNode* parent;
        while ((parent = get_parent(node)) && parent->left == node) {
            node = parent;
        }
        return parent;
    }
}

RbtNode* rbtree_next(RbtNode* node)
{
    assert(node != NULL);

    if (node->right)
        return get_min(node->right);
    else {
        RbtNode* parent = NULL;
        while ((parent = get_parent(node)) != NULL && parent->right == node) {
            node = parent;
        }
        return parent;
    }
}

RbtNode* rbtree_createnode(uintptr_t key, void* data)
{
    RbtNode* newnode = slab_alloc(&rbt_factory.rbtnode_ele_allocator);
    if (newnode == NULL)
        return NULL;

    newnode->key = key;
    newnode->data = data;
    newnode->parent = NULL;
    newnode->left = NULL;
    newnode->right = NULL;
    return newnode;
}

static inline int compare(uintptr_t key_a, uintptr_t key_b)
{
    if (key_a > key_b)
        return 1;
    else if (key_a == key_b)
        return 0;
    else
        return -1;
}

RbtNode* do_lookup(uintptr_t key,
    RbtTree* tree,
    RbtNode** pparent)
{
    RbtNode* current = tree->root;

    while (current) {
        int ret = compare(current->key, key);
        if (ret == 0)
            return current;
        else {
            if (pparent != NULL) {
                *pparent = current;
            }
            if (ret < 0)
                current = current->right;
            else
                current = current->left;
        }
    }
    return NULL;
}

RbtNode* rbt_search(RbtTree* tree, uintptr_t key)
{
    RbtNode* node;
    node = do_lookup(key, tree, NULL);
    return node;
}

static void set_child(RbtTree* tree, RbtNode* node, RbtNode* child)
{
    int ret = compare(node->key, child->key);
    assert(ret != 0);

    if (ret > 0) {
        node->left = child;
    } else {
        node->right = child;
    }
}

static void rotate_left(RbtNode* node, RbtTree* tree)
{
    RbtNode* p = node;
    RbtNode* q = node->right;
    RbtNode* parent = node->parent;
    if (parent == NULL) {
        tree->root = q;
    } else {
        if (parent->left == p)
            parent->left = q;
        else
            parent->right = q;
    }
    set_parent(parent, q);
    set_parent(q, p);

    p->right = q->left;
    if (q->left)
        set_parent(p, q->left);
    q->left = p;
}

static void rotate_right(RbtNode* node, RbtTree* tree)
{
    RbtNode* p = node;
    RbtNode* q = node->left; /* can't be NULL */
    RbtNode* parent = get_parent(p);

    if (!is_root(p)) {
        if (parent->left == p)
            parent->left = q;
        else
            parent->right = q;
    } else
        tree->root = q;
    set_parent(parent, q);
    set_parent(q, p);

    p->left = q->right;
    if (p->left)
        set_parent(p, p->left);
    q->right = p;
}

void rbtree_init(RbtTree* tree)
{
    tree->root = NULL;
    tree->nr_ele = 0;
}

RbtNode* __rbtree_insert(RbtNode* node, RbtTree* tree)
{
    RbtNode* samenode = NULL;
    RbtNode* parent = NULL;

    samenode = do_lookup(node->key, tree, &parent);
    if (samenode != NULL)
        return samenode;

    node->left = node->right = NULL;
    set_color(RED, node);
    set_parent(parent, node);

    if (parent == NULL)
        tree->root = node;
    else {
        set_child(tree, parent, node);
    }

    while ((parent = get_parent(node)) != NULL && parent->color == RED) {
        RbtNode* grandpa = get_parent(parent); // grandpa must be existed
        // because root is black ,and parent is red,
        // parent can not be root of tree. and parent is red,so grandpa must be black
        if (parent == grandpa->left) {
            RbtNode* uncle = grandpa->right;
            if (uncle && get_color(uncle) == RED) {
                set_color(RED, grandpa);
                set_color(BLACK, parent);
                set_color(BLACK, uncle);
                node = grandpa;
            } else {
                if (node == parent->right) {
                    rotate_left(parent, tree);
                    node = parent;
                    parent = get_parent(parent);
                }
                set_color(BLACK, parent);
                set_color(RED, grandpa);
                rotate_right(grandpa, tree);
            }

        } else {
            RbtNode* uncle = grandpa->left;
            if (uncle && uncle->color == RED) {
                set_color(RED, grandpa);
                set_color(BLACK, parent);
                set_color(BLACK, uncle);
                node = grandpa;
            } else {
                if (node == parent->left) {
                    rotate_right(parent, tree);
                    node = parent;
                    parent = get_parent(node);
                }
                set_color(BLACK, parent);
                set_color(RED, grandpa);
                rotate_left(grandpa, tree);
            }
        }
    }

    set_color(BLACK, tree->root);
    return NULL;
}

int rbt_insert(RbtTree* tree, uintptr_t key, void* data)
{
    if (rbt_search(tree, key) != NULL) {
        return RBTTREE_INSERT_EXISTED;
    }

    RbtNode* node = rbtree_createnode(key, data);
    RbtNode* samenode = NULL;
    if (node == NULL)
        return RBTTREE_INSERT_FAILED;
    else
        samenode = __rbtree_insert(node, tree);

    assert(samenode == NULL);

    tree->nr_ele++;
    return RBTTREE_INSERT_SECC;
}

void replace_node(RbtTree* t, RbtNode* oldn, RbtNode* newn)
{
    if (oldn->parent == NULL) {
        t->root = newn;
    } else {
        if (oldn == oldn->parent->left)
            oldn->parent->left = newn;
        else
            oldn->parent->right = newn;
    }
    if (newn != NULL) {
        newn->parent = oldn->parent;
    }
}

void delete_case1(RbtTree* tree, RbtNode* node)
{
    if (node->parent == NULL)
        return;
    else
        delete_case2(tree, node);
}

void delete_case2(RbtTree* tree, RbtNode* node)
{
    if (get_color(sibling(node)) == RED) {
        node->parent->color = RED;
        sibling(node)->color = BLACK;
        if (node == node->parent->left) {
            rotate_left(node->parent, tree);
        } else {
            rotate_right(node->parent, tree);
        }
    }
    delete_case3(tree, node);
}

void delete_case3(RbtTree* tree, RbtNode* node)
{
    if (node->parent->color == BLACK && get_color(sibling(node)) == BLACK && get_color(sibling(node)->right) == BLACK && get_color(sibling(node)->left) == BLACK) {
        sibling(node)->color = RED;
        delete_case1(tree, node->parent);
    } else {
        delete_case4(tree, node);
    }
}

void delete_case4(RbtTree* t, RbtNode* n)
{
    if (get_color(n->parent) == RED && get_color(sibling(n)) == BLACK && get_color(sibling(n)->left) == BLACK && get_color(sibling(n)->right) == BLACK) {
        sibling(n)->color = RED; // sibling's two son is black ,so it can changed to red
        n->parent->color = BLACK;
    } else
        delete_case5(t, n);
}

void delete_case5(RbtTree* t, RbtNode* n)
{
    if (n == n->parent->left && get_color(sibling(n)) == BLACK && get_color(sibling(n)->left) == RED && get_color(sibling(n)->right) == BLACK) {
        sibling(n)->color = RED;
        sibling(n)->left->color = BLACK;
        rotate_right(sibling(n), t);
    } else if (n == n->parent->right && get_color(sibling(n)) == BLACK && get_color(sibling(n)->right) == RED && get_color(sibling(n)->left) == BLACK) {
        sibling(n)->color = RED;
        sibling(n)->right->color = BLACK;
        rotate_left(sibling(n), t);
    }
    delete_case6(t, n);
}

void delete_case6(RbtTree* t, RbtNode* n)
{
    sibling(n)->color = get_color(n->parent);
    n->parent->color = BLACK;
    if (n == n->parent->left) {
        assert(get_color(sibling(n)->right) == RED);
        sibling(n)->right->color = BLACK;
        rotate_left(n->parent, t);
    } else {
        assert(get_color(sibling(n)->left) == RED);
        sibling(n)->left->color = BLACK;
        rotate_right(n->parent, t);
    }
}

void __rbtree_remove(RbtNode* node, RbtTree* tree)
{
    RbtNode* left = node->left;
    RbtNode* right = node->right;
    RbtNode* child = NULL;
    if (left != NULL && right != NULL) {
        RbtNode* next = get_min(right);
        node->key = next->key;
        node->data = next->data;
        node = next;
    }

    assert(node->left == NULL || node->right == NULL);
    child = (node->right == NULL ? node->left : node->right);
    if (get_color(node) == BLACK) {
        set_color(get_color(child), node);
        delete_case1(tree, node);
    }
    replace_node(tree, node, child);
    if (node->parent == NULL && child != NULL) // node is root,root should be black
        set_color(BLACK, child);
    slab_free(&rbt_factory.rbtnode_ele_allocator, (void*)node);
}

int rbt_delete(RbtTree* tree, uintptr_t key)
{
    RbtNode* node = do_lookup(key, tree, NULL);
    if (node == NULL)
        return RBTTREE_DELETE_FAILED;
    else
        __rbtree_remove(node, tree);

    tree->nr_ele--;
    if (rbt_is_empty(tree)) {
        assert(tree->root == NULL);
    }
    return RBTTREE_DELETE_SUCC;
}

void rbt_traverse_inner(RbtNode* node, rbt_traverse_fn fn, void* data)
{
    if (node == NULL) {
        return;
    }

    if (fn(node, data)) {
        rbt_traverse_inner(node->left, fn, data);
        rbt_traverse_inner(node->right, fn, data);
    }
}

void rbt_traverse(RbtTree* tree, rbt_traverse_fn fn, void* data)
{
    rbt_traverse_inner(tree->root, fn, data);
}