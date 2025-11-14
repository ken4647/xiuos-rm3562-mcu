

#include "actracer.h"
#include "assert.h"

#include "queue.h"

struct QueueFactory {
    TraceTag tag;
    struct slab_allocator queue_ele_allocator;
};
static struct QueueFactory queue_factory;

void module_queue_factory_init(TraceTag* _softkernel_tag)
{
    CreateResourceTag(&queue_factory.tag, _softkernel_tag, "GlobalQueueFactory", TRACER_SYSOBJECT, &queue_factory);
    slab_init(&queue_factory.queue_ele_allocator, sizeof(struct QueueNode), "QueueNodeAllocator");
}

void queue_init(Queue* queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->nr_ele = 0;
}

struct QueueNode* queue_front(Queue* queue)
{
    return queue->front;
}

bool queue_is_empty(Queue* queue)
{
    if (queue->front == NULL) {
        assert(queue->nr_ele == 0);
        return true;
    }

    return false;
}

bool dequeue(Queue* queue)
{
    struct QueueNode* temp = queue->front;

    if (queue->front == NULL) {
        return false;
    }

    if (queue->front == queue->rear)
        queue->front = queue->rear = NULL;
    else
        queue->front = queue->front->next;

    queue->nr_ele--;
    slab_free(&queue_factory.queue_ele_allocator, (void*)temp);
    return true;
}

bool enqueue(Queue* queue, uintptr_t key, void* data)
{
    QueueNode* temp = (struct QueueNode*)slab_alloc(&queue_factory.queue_ele_allocator);
    if (temp == NULL) {
        return false;
    }
    temp->key = key;
    temp->data = data;
    temp->next = NULL;

    if (queue->front == NULL && queue->rear == NULL) {
        queue->front = queue->rear = temp;
    } else {
        queue->rear->next = temp;
        queue->rear = temp;
    }

    queue->nr_ele++;
    return true;
}