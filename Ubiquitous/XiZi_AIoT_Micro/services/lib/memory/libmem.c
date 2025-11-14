/*
 * mm.c - malloc using segregated list
 * KAIST
 * Tony Kim
 *
 * In this approach,
 * Every block has a header and a footer
 * in which header contains reallocation information, size, and allocation info
 * and footer contains size and allocation info.
 * Free list are tagged to the segregated list.
 * Therefore all free block contains pointer to the predecessor and successor.
 * The segregated list headers are organized by 2^k size.
 *
 */

#include "libmem.h"
#include "usyscall.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

// My additional Macros
#define WSIZE 4 // word and header/footer size (bytes)
#define DSIZE 8 // double word size (bytes)
#define INITCHUNKSIZE (1 << 6)
#define CHUNKSIZE (1 << 12) //+(1<<7)

#define LISTLIMIT 20

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

// Pack a size and allocated bit into a word
#define PACK(size, alloc) ((size) | (alloc))

// Read and write a word at address p
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val) | GET_TAG(p))
#define PUT_NOTAG(p, val) (*(unsigned int*)(p) = (val))

// Store predecessor or successor pointer for free blocks
#define SET_PTR(p, ptr) (*(uintptr_t*)(p) = (uintptr_t)(ptr))

// Read the size and allocation bit from address p
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_TAG(p) (GET(p) & 0x2)
#define SET_RATAG(p) (GET(p) |= 0x2)
#define REMOVE_RATAG(p) (GET(p) &= ~0x2)

// Address of block's header and footer
#define HDRP(ptr) ((char*)(ptr)-WSIZE)
#define FTRP(ptr) ((char*)(ptr) + GET_SIZE(HDRP(ptr)) - DSIZE)

// Address of (physically) next and previous blocks
#define NEXT_BLKP(ptr) ((char*)(ptr) + GET_SIZE((char*)(ptr)-WSIZE))
#define PREV_BLKP(ptr) ((char*)(ptr)-GET_SIZE((char*)(ptr)-DSIZE))

// Address of free block's predecessor and successor entries
#define PRED_PTR(ptr) ((char*)(ptr))
#define SUCC_PTR(ptr) ((char*)(ptr) + WSIZE)

// Address of free block's predecessor and successor on the segregated list
#define PRED(ptr) (*(char**)(ptr))
#define SUCC(ptr) (*(char**)(SUCC_PTR(ptr)))

// End of my additional macros

// Global var
void* segregated_free_lists[LISTLIMIT];

// Functions
static void* extend_heap(size_t size);
static void* coalesce(void* ptr);
static void* place(void* ptr, size_t asize);
static void insert_node(void* ptr, size_t size);
static void delete_node(void* ptr);

static uintptr_t userland_heap_base;
static uintptr_t userland_heap_top;
static uintptr_t requested_heap_size;

static void* mem_sbrk(size_t size)
{
    uintptr_t userland_heap_size = userland_heap_top - userland_heap_base;
    if (userland_heap_size - requested_heap_size >= size) {
        void* ret_ptr = (void*)(userland_heap_base + requested_heap_size);
        requested_heap_size += size;
        return ret_ptr;
    }

    uintptr_t size_needed = size - (userland_heap_size - requested_heap_size);
    userland_heap_top = mmap(userland_heap_top, (uintptr_t)NULL, size_needed, false);

    return mem_sbrk(size);
}

static void* extend_heap(size_t size)
{
    void* ptr;
    size_t asize; // Adjusted size

    asize = ALIGN(size);

    if ((ptr = mem_sbrk(asize)) == (void*)-1)
        return NULL;

    // Set headers and footer
    PUT_NOTAG(HDRP(ptr), PACK(asize, 0));
    PUT_NOTAG(FTRP(ptr), PACK(asize, 0));
    PUT_NOTAG(HDRP(NEXT_BLKP(ptr)), PACK(0, 1));
    insert_node(ptr, asize);

    return coalesce(ptr);
}

static void insert_node(void* ptr, size_t size)
{
    int list = 0;
    void* search_ptr = ptr;
    void* insert_ptr = NULL;

    // Select segregated list
    while ((list < LISTLIMIT - 1) && (size > 1)) {
        size >>= 1;
        list++;
    }

    // Keep size ascending order and search
    search_ptr = segregated_free_lists[list];
    while ((search_ptr != NULL) && (size > GET_SIZE(HDRP(search_ptr)))) {
        insert_ptr = search_ptr;
        search_ptr = PRED(search_ptr);
    }

    // Set predecessor and successor
    if (search_ptr != NULL) {
        if (insert_ptr != NULL) {
            SET_PTR(PRED_PTR(ptr), search_ptr);
            SET_PTR(SUCC_PTR(search_ptr), ptr);
            SET_PTR(SUCC_PTR(ptr), insert_ptr);
            SET_PTR(PRED_PTR(insert_ptr), ptr);
        } else {
            SET_PTR(PRED_PTR(ptr), search_ptr);
            SET_PTR(SUCC_PTR(search_ptr), ptr);
            SET_PTR(SUCC_PTR(ptr), NULL);
            segregated_free_lists[list] = ptr;
        }
    } else {
        if (insert_ptr != NULL) {
            SET_PTR(PRED_PTR(ptr), NULL);
            SET_PTR(SUCC_PTR(ptr), insert_ptr);
            SET_PTR(PRED_PTR(insert_ptr), ptr);
        } else {
            SET_PTR(PRED_PTR(ptr), NULL);
            SET_PTR(SUCC_PTR(ptr), NULL);
            segregated_free_lists[list] = ptr;
        }
    }

    return;
}

static void delete_node(void* ptr)
{
    int list = 0;
    size_t size = GET_SIZE(HDRP(ptr));

    // Select segregated list
    while ((list < LISTLIMIT - 1) && (size > 1)) {
        size >>= 1;
        list++;
    }

    if (PRED(ptr) != NULL) {
        if (SUCC(ptr) != NULL) {
            SET_PTR(SUCC_PTR(PRED(ptr)), SUCC(ptr));
            SET_PTR(PRED_PTR(SUCC(ptr)), PRED(ptr));
        } else {
            SET_PTR(SUCC_PTR(PRED(ptr)), NULL);
            segregated_free_lists[list] = PRED(ptr);
        }
    } else {
        if (SUCC(ptr) != NULL) {
            SET_PTR(PRED_PTR(SUCC(ptr)), NULL);
        } else {
            segregated_free_lists[list] = NULL;
        }
    }

    return;
}

static void* coalesce(void* ptr)
{
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(ptr)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
    size_t size = GET_SIZE(HDRP(ptr));

    // Do not coalesce with previous block if the previous block is tagged with Reallocation tag
    if (GET_TAG(HDRP(PREV_BLKP(ptr))))
        prev_alloc = 1;

    if (prev_alloc && next_alloc) { // Case 1
        return ptr;
    } else if (prev_alloc && !next_alloc) { // Case 2
        delete_node(ptr);
        delete_node(NEXT_BLKP(ptr));
        size += GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        PUT(HDRP(ptr), PACK(size, 0));
        PUT(FTRP(ptr), PACK(size, 0));
    } else if (!prev_alloc && next_alloc) { // Case 3
        delete_node(ptr);
        delete_node(PREV_BLKP(ptr));
        size += GET_SIZE(HDRP(PREV_BLKP(ptr)));
        PUT(FTRP(ptr), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(ptr)), PACK(size, 0));
        ptr = PREV_BLKP(ptr);
    } else { // Case 4
        delete_node(ptr);
        delete_node(PREV_BLKP(ptr));
        delete_node(NEXT_BLKP(ptr));
        size += GET_SIZE(HDRP(PREV_BLKP(ptr))) + GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        PUT(HDRP(PREV_BLKP(ptr)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(ptr)), PACK(size, 0));
        ptr = PREV_BLKP(ptr);
    }

    insert_node(ptr, size);

    return ptr;
}

static void* place(void* ptr, size_t asize)
{
    size_t ptr_size = GET_SIZE(HDRP(ptr));
    size_t remainder = ptr_size - asize;

    delete_node(ptr);

    if (remainder <= DSIZE * 2) {
        // Do not split block
        PUT(HDRP(ptr), PACK(ptr_size, 1));
        PUT(FTRP(ptr), PACK(ptr_size, 1));
    }

    else if (asize >= 100) {
        // Split block
        PUT(HDRP(ptr), PACK(remainder, 0));
        PUT(FTRP(ptr), PACK(remainder, 0));
        PUT_NOTAG(HDRP(NEXT_BLKP(ptr)), PACK(asize, 1));
        PUT_NOTAG(FTRP(NEXT_BLKP(ptr)), PACK(asize, 1));
        insert_node(ptr, remainder);
        return NEXT_BLKP(ptr);

    }

    else {
        // Split block
        PUT(HDRP(ptr), PACK(asize, 1));
        PUT(FTRP(ptr), PACK(asize, 1));
        PUT_NOTAG(HDRP(NEXT_BLKP(ptr)), PACK(remainder, 0));
        PUT_NOTAG(FTRP(NEXT_BLKP(ptr)), PACK(remainder, 0));
        insert_node(NEXT_BLKP(ptr), remainder);
    }
    return ptr;
}

static bool malloc_inited = false;
int mm_init(void)
{
    // init heap
    userland_heap_base = task_heap_base();
    userland_heap_top = userland_heap_base;
    requested_heap_size = 0;

    int list;
    char* heap_start; // Pointer to beginning of heap

    // Initialize segregated free lists
    for (list = 0; list < LISTLIMIT; list++) {
        segregated_free_lists[list] = NULL;
    }

    // Allocate memory for the initial empty heap
    if ((long)(heap_start = mem_sbrk(4 * WSIZE)) == -1)
        return -1;

    PUT_NOTAG(heap_start, 0); /* Alignment padding */
    PUT_NOTAG(heap_start + (1 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT_NOTAG(heap_start + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT_NOTAG(heap_start + (3 * WSIZE), PACK(0, 1)); /* Epilogue header */

    if (extend_heap(INITCHUNKSIZE) == NULL)
        return -1;

    malloc_inited = true;
    return 0;
}

void* malloc(size_t size)
{
    while (!malloc_inited) {
        mm_init();
    }

    size_t asize; /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    void* ptr = NULL; /* Pointer */

    // Ignore size 0 cases
    if (size == 0)
        return NULL;

    // Align block size
    if (size <= DSIZE) {
        asize = 2 * DSIZE;
    } else {
        asize = ALIGN(size + DSIZE);
    }

    int list = 0;
    size_t searchsize = asize;
    // Search for free block in segregated list
    while (list < LISTLIMIT) {
        if ((list == LISTLIMIT - 1) || ((searchsize <= 1) && (segregated_free_lists[list] != NULL))) {
            ptr = segregated_free_lists[list];
            // Ignore blocks that are too small or marked with the reallocation bit
            while ((ptr != NULL) && ((asize > GET_SIZE(HDRP(ptr))) || (GET_TAG(HDRP(ptr))))) {
                ptr = PRED(ptr);
            }
            if (ptr != NULL)
                break;
        }

        searchsize >>= 1;
        list++;
    }

    // if free block is not found, extend the heap
    if (ptr == NULL) {
        extendsize = MAX(asize, CHUNKSIZE);

        if ((ptr = extend_heap(extendsize)) == NULL)
            return NULL;
    }

    // Place and divide block
    ptr = place(ptr, asize);

    // Return pointer to newly allocated block
    return ptr;
}

void free(void* ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    REMOVE_RATAG(HDRP(NEXT_BLKP(ptr)));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));

    insert_node(ptr, size);
    coalesce(ptr);

    return;
}
