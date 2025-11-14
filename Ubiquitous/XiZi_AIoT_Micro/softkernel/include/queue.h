#pragma once
#include <stddef.h>

typedef struct QueueNode {
    uintptr_t key;
    void* data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int nr_ele;
} Queue;

void queue_init(Queue* queue);
QueueNode* queue_front(Queue* queue);
bool queue_is_empty(Queue* queue);
bool dequeue(Queue* queue);
bool enqueue(Queue* queue, uintptr_t key, void* data);

void module_queue_factory_init(TraceTag* _softkernel_tag);