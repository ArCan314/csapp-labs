/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static char *heap_start = NULL;
static char *prologue_header = NULL;
static char *epilogue_header = NULL;
static char *free_list_head = NULL;
static char *free_list_tail = NULL;

static const uint32_t kChunkSize = 64u * 32u;
static const uint32_t kHeaderSize = 4u;
static const uint32_t kFreeListNodeSize = 8u;
static const uint32_t kMinimumPayloadSize = kChunkSize / 16u;

// |  header  |  payload  |  padding  |  footer  | ALLOCATED BLOCK
// |    4     |     n     |     8     |    4     |
// size = 16 + n

// |  header  |   payload  |  prev-free-block  |  next-free-block  |  footer  |
// |    4     |      n     |        4          |         4         |    4     |
// size = 16 + n

// |    size   |   allocated-bit   |
// |    29     |         3         |
static uint32_t is_allocated(const char *header);
static uint32_t get_size(const char *header);

static uint32_t *get_footer(const char *header);
static void set_header(char *header, const size_t size, const size_t is_allocated);
static void set_footer(char *header, const size_t size, const size_t is_allocated);

static char *next_free_block(char *header);
static char *prev_free_block(char *header);
static uintptr_t *get_free_list_node(char *header);
static void remove_free_list_node(char *header);
static void insert_free_list_node(char *header);
static void set_prev_free_block(char *header, const char *prev_free_block_header);
static void set_next_free_block(char *header, const char *next_free_block_header);
static int free_list_check(void);

static char *find_fit(size_t size);

static char *next(const char *header);
static char *prev(const char *header);
static int is_end(const char *header);
static void collapse(char *header);
static int expand(const size_t size);
static size_t max(size_t a, size_t b);

static void address_assert(void *payload_addr);
static int free_list_assert(char *header);
static int heap_assert(char *payload);

static void *ptr_min(void *a, void *b);
static void dbg_printf(const char *format, ...);
static void dbg_print_heap(void);
static void dbg_print_free_list(void);

static uint32_t is_allocated(const char *header)
{
    assert(header);
    return *(uint32_t *)header & 0x7;
}

static uint32_t get_size(const char *header)
{
    assert(header);
    return *(uint32_t *)header & ~0x7;
}

static uint32_t *get_footer(const char *header)
{
    assert(header);
    return (uint32_t *)(header - kHeaderSize + get_size(header));
}

static void set_header(char *header, const size_t size, const size_t is_allocated)
{
    assert(header);
    *((uint32_t *)header) = size | is_allocated;
}

static void set_footer(char *header, const size_t size, const size_t is_allocated)
{
    assert(header);
    *get_footer(header) = size | is_allocated;
}

static char *next_free_block(char *header)
{
    free_list_assert(header);

    char *res = (char *)get_free_list_node(header)[1];

    assert((header == free_list_tail) ? (res == NULL) : 1);
    assert((res) ? (free_list_assert(res)) : (1));

    return res;
}

static char *prev_free_block(char *header)
{
    free_list_assert(header);

    char *res = (char *)get_free_list_node(header)[0];

    assert((header == free_list_head) ? (res == NULL) : 1);
    assert((res) ? (free_list_assert(res)) : (1));

    return res;
}

static uintptr_t *get_free_list_node(char *header)
{
    free_list_assert(header);

    uintptr_t *res = (header + get_size(header) - kHeaderSize - kFreeListNodeSize);

    assert((free_list_head == header) ? (res[0] == NULL) : (1));
    assert((free_list_tail == header) ? (res[1] == NULL) : (1));
    // assert((res[0]) ? (free_list_assert(res[0])) : (1));
    // assert((res[1]) ? (free_list_assert(res[1])) : (1));

    return res;
}

static void insert_free_list_node(char *header)
{
    free_list_assert(header);

    if (!free_list_head || !free_list_tail)
        assert(!free_list_head && !free_list_tail);

    if (!free_list_head)
    {
#ifdef DEBUG
        set_next_free_block(header, NULL);
        set_prev_free_block(header, NULL);
#endif
        memset(get_free_list_node(header), 0, 2 * sizeof(uintptr_t));
        free_list_head = free_list_tail = header;
    }
    else
    {
        char *cur = free_list_head;
        while (cur && (uintptr_t)cur < (uintptr_t)header)
            cur = next_free_block(cur);

        char *pre = cur ? prev_free_block(cur) : free_list_tail;

        set_prev_free_block(header, pre);
        set_next_free_block(header, cur);

        if (!pre) // | cur(head) |  -->  | header(head) | cur |
        {
            set_prev_free_block(cur, header);
            free_list_head = header;
        }
        else if (!cur) // | pre(tail) |  -->  | pre | header(tail) |
        {
            set_next_free_block(pre, header);
            free_list_tail = header;
        }
        else // | pre | cur |  -->  | pre | header | cur |
        {
            set_next_free_block(pre, header);
            set_prev_free_block(cur, header);
        }

        // if (free_list_head == free_list_tail)
        // {
        //     assert((uintptr_t)cur != (uintptr_t)header);

        //     if ((uintptr_t)cur < (uintptr_t)header)
        //     {
        //         set_next_free_block(cur, header);
        //         set_prev_free_block(header, cur);
        //         set_next_free_block(header, NULL);
        //         free_list_tail = header;
        //     }
        //     else
        //     {
        //         set_next_free_block(header, cur);
        //         set_prev_free_block(header, NULL);
        //         set_prev_free_block(cur, header);
        //         free_list_head = header;
        //     }
        // }
        // else if (cur == free_list_tail)
        // {
        //     assert((uintptr_t)cur != (uintptr_t)header);
        //     if ((uintptr_t)cur < (uintptr_t)header)
        //     {
        //         set_next_free_block(cur, header);
        //         set_prev_free_block(header, cur);
        //         set_next_free_block(header, NULL);
        //         free_list_tail = header;
        //     }
        //     else
        //     {
        //         set_next_free_block(header, cur);
        //         set_prev_free_block(header, NULL);
        //         set_prev_free_block(cur, header);
        //     }
        // }
        // else if (cur == free_list_head)
        // {
        //     set_next_free_block(header, cur);
        //     set_prev_free_block(header, NULL);
        //     set_prev_free_block(cur, header);
        //     free_list_head = header;
        // }
        // else
        // {
        //     set_next_free_block(pre, header);
        //     set_prev_free_block(cur, header);
        //     set_next_free_block(header, cur);
        //     set_prev_free_block(header, pre);
        // }
    }
}

static void remove_free_list_node(char *header)
{
    free_list_assert(header);

    char *next = next_free_block(header);
    char *prev = prev_free_block(header);

    if (next && prev)
    {
        set_prev_free_block(next, prev);
        set_next_free_block(prev, next);
    }
    else if (next && !prev)
    {
        set_prev_free_block(next, NULL);
        free_list_head = next;
    }
    else if (!next && prev)
    {
        set_next_free_block(prev, NULL);
        free_list_tail = prev;
    }
    else
    {
        free_list_head = NULL;
        free_list_tail = NULL;
    }
}

static void set_prev_free_block(char *header, const char *prev_free_block_header)
{
    free_list_assert(header);
    assert(prev_free_block_header ? (free_list_assert(prev_free_block_header)) : (1));

    get_free_list_node(header)[0] = prev_free_block_header;
}

static void set_next_free_block(char *header, const char *next_free_block_header)
{
    free_list_assert(header);
    assert(next_free_block_header ? (free_list_assert(next_free_block_header)) : (1));

    get_free_list_node(header)[1] = next_free_block_header;
}

static char *find_fit(size_t size)
{
    assert(free_list_head ? free_list_assert(free_list_head) : (1));
    char *cur = free_list_head;

    while (cur)
    {
        if (get_size(cur) >= size)
            break;
        cur = next_free_block(cur);
    }
    return cur ? cur : (char *)-1;
}

static char *next(const char *header)
{
    assert(header);
    assert(get_size(header) != 0);
    return (char *)header + get_size(header);
}

static char *prev(const char *header)
{
    assert(header);
    assert(get_size(header) != 0);
    uint32_t prev_size = (*(uint32_t *)(header - 4)) & ~0x7;
    return (char *)header - prev_size;
}

static int is_end(const char *header)
{
    return header == epilogue_header;
}

static void collapse(char *header)
{
    free_list_assert(header);

    char *pre = prev(header);
    char *nxt = next(header);

    address_assert(pre);
    address_assert(header);
    address_assert(nxt);

    int pre_allocated = is_allocated(pre);
    int nxt_allocated = is_allocated(nxt);

    int pre_size = get_size(pre);
    int cur_size = get_size(header);
    int nxt_size = get_size(nxt);

    assert(pre_allocated ? (1) : (free_list_assert(pre)));
    assert(nxt_allocated ? (1) : (free_list_assert(nxt)));

    if (pre_allocated && nxt_allocated)
    {
        if (cur_size < kMinimumPayloadSize + 2 * kHeaderSize + kFreeListNodeSize &&
            pre != prologue_header)
        {
            // |PF|...|PRE|CUR|NXT|...|NF| --> |PF|...|PRE+CUR|NXT|...|NF|
            remove_free_list_node(header);
            set_header(pre, cur_size + pre_size, 1);
            set_footer(header, cur_size + pre_size, 1);
        }
    }
    else if (pre_allocated && !nxt_allocated)
    {
        char *pre_free_block = prev_free_block(header);
        char *nxt_nxt_free_block = next_free_block(nxt);
        //  |h|  PREV  |p1|n1|f|    ...   |h|  HEADER   |    NXT   |p2|n2|f|   ...   |h|  NXT_NXT  |p3|n3|f|
        // n1 --> header    NO CHANGE
        // p2 --> header    TO      p2 --> PREV
        // n2 --> NXT_NXT   NO CHANGE
        // p3 --> NXT       TO      p3 --> header

        set_prev_free_block(nxt, pre_free_block);
        if (nxt_nxt_free_block)
            set_prev_free_block(nxt_nxt_free_block, header);

        if (nxt == free_list_tail)
            free_list_tail = header;

        (*(uint32_t *)header) += nxt_size;
        *get_footer(nxt) += cur_size;
    }
    else if (!pre_allocated && nxt_allocated)
    {
        char *pre_pre_free_block = prev_free_block(pre);
        char *nxt_free_block = next_free_block(header);

        //  |h|  PREV_PREV  |p1|n1|f|    ...   |h|  PREV   |    HEADER   |p2|n2|f|   ...   |h|  NXT  |p3|n3|f|
        // n1 --> prev      NO CHANGE
        // p2 --> prev      TO      p2 --> PREV_PREV
        // n2 --> NXT       NO CHANGE
        // p3 --> header    TO      p3 --> PREV

        set_prev_free_block(header, pre_pre_free_block);
        if (nxt_free_block)
            set_prev_free_block(nxt_free_block, pre);

        if (free_list_tail == header)
            free_list_tail = pre;

        (*(uint32_t *)pre) += cur_size;
        *get_footer(header) += pre_size;
    }
    else if (!pre_allocated && !nxt_allocated)
    {
        char *pre_pre_free_block = prev_free_block(pre);
        char *nxt_nxt_free_block = next_free_block(nxt);

        //  |h|  PREV_PREV  |p1|n1|f|    ...   |h|  PREV   |    HEADER  |  NXT  |p2|n2|f|   ...   |h|  NXT_NXT  |p3|n3|f|
        // n1 --> prev      NO CHANGE
        // p2 --> header    TO      p2 --> PREV_PREV
        // n2 --> NXT_NXT   NO CHANGE
        // p3 --> NXT       TO      p3 --> PREV

        set_prev_free_block(nxt, pre_pre_free_block);
        if (nxt_nxt_free_block)
            set_prev_free_block(nxt_nxt_free_block, pre);

        if (free_list_tail == nxt)
            free_list_tail = pre;

        (*(uint32_t *)pre) += cur_size + nxt_size;
        *get_footer(nxt) += cur_size + pre_size;
    }

    assert(free_list_head ? (free_list_assert(free_list_head)) : (1));
    assert(free_list_tail ? (free_list_assert(free_list_tail)) : (1));
}

static int expand(const size_t size)
{
    if (mem_sbrk(size) == (void *)-1)
        return -1;

    set_header(epilogue_header, size, 0);
    set_footer(epilogue_header, size, 0);
    insert_free_list_node(epilogue_header);

    char *before_epi = epilogue_header;
    epilogue_header = next(epilogue_header);
    set_header(epilogue_header, 0, 1);

    collapse(before_epi);
    return 0;
}

static size_t max(size_t a, size_t b)
{
    return a < b ? b : a;
}

static void address_assert(void *header)
{
    assert(((uintptr_t)((char *)header + 4)) % ALIGNMENT == 0);
}

static void dbg_print_heap(void)
{
#ifdef DEBUG
    putchar('\t');
    char *cur = prologue_header;
    printf("[%p,s=%d,a=%d,PRO]->", cur, get_size(cur), is_allocated(cur));
    fflush(NULL);
    cur = next(cur);

    while (!is_end(cur))
    {
        printf("[%p,s=%d,a=%d]->", cur, get_size(cur), is_allocated(cur));
        fflush(NULL);
        cur = next(cur);
    }
    printf("[%p,s=%d,a=%d,EPI]\n", cur, get_size(cur), is_allocated(cur));
    fflush(NULL);
#endif
}

static void dbg_printf(const char *format, ...)
{
#ifdef DEBUG
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    fflush(NULL);
    va_end(args);
#endif
}

static void *ptr_min(void *a, void *b)
{
    return (uintptr_t)a < (uintptr_t)b ? a : b;
}

static int free_list_assert(char *header)
{
    assert(header);
    assert(get_size(header) >= 2 * kHeaderSize + kFreeListNodeSize);
    assert(!is_allocated(header));
    return 1;
}

static void dbg_print_free_list(void)
{
#ifdef DEBUG
    printf("\tFREE-LIST: ");
    fflush(NULL);

    char *cur = free_list_head;

    if (!cur)
        printf("NO FREE LIST NODE\n");
    else
    {
        while (cur != free_list_tail)
        {
            printf("[%p,s=%d,a=%d,p=%p,n=%p]->", cur, get_size(cur), is_allocated(cur), prev_free_block(cur), next_free_block(cur));
            fflush(NULL);
            cur = next_free_block(cur);
        }
        printf("[%p,s=%d,a=%d,p=%p,n=%p]\n", cur, get_size(cur), is_allocated(cur), prev_free_block(cur), next_free_block(cur));
        fflush(NULL);
    }
#endif
}

static int free_list_check(void)
{
#ifdef DEBUG
    char *cur = free_list_head, *pre = NULL;
    while (cur)
    {
        free_list_assert(cur);
        assert(get_free_list_node(cur)[0] == pre);
        pre = cur;
        cur = next_free_block(cur);
    }

    cur = free_list_tail, pre = NULL;
    while (cur)
    {
        free_list_assert(cur);
        assert(get_free_list_node(cur)[1] == pre);
        pre = cur;
        cur = prev_free_block(cur);
    }

    char *m_cur = prologue_header;
    cur = free_list_head;

    while (!is_end(m_cur))
    {
        if (!is_allocated(m_cur))
        {
            free_list_assert(m_cur);
            assert(cur == m_cur);
            cur = next_free_block(cur);
        }
        m_cur = next(m_cur);
    }
#endif
}

static int heap_assert(char *payload)
{
    assert(payload < mem_heap_hi());
    assert(epilogue_header < mem_heap_hi());
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    dbg_printf("MM_INIT CALLED\n");
    assert(sizeof(uintptr_t) == sizeof(void *));
    assert(sizeof(uint32_t) == 4);

    heap_start = (char *)mem_sbrk(3 * kHeaderSize + kFreeListNodeSize);
    if (heap_start == (char *)-1)
    {
        dbg_printf("\tMM_INIT FAILED\n");
        return -1;
    }

    char *prologue_payload = (char *)ALIGN(((uintptr_t)heap_start) + kHeaderSize);
    prologue_header = prologue_payload - kHeaderSize;
    address_assert(prologue_header);

    if ((intptr_t)mem_sbrk(prologue_header - heap_start) == -1)
    {
        dbg_printf("\tMM_INIT FAILED\n");
        return -1;
    }

    set_header(prologue_header, 2 * kHeaderSize + kFreeListNodeSize, 1);
    set_footer(prologue_header, 2 * kHeaderSize + kFreeListNodeSize, 1);

    epilogue_header = next(prologue_header);
    address_assert(epilogue_header);

    set_header(epilogue_header, 0, 1);

    free_list_head = free_list_tail = NULL;

    dbg_print_heap();
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    free_list_check();
    size_t allocate_size = ALIGN(size + 2 * kHeaderSize + kFreeListNodeSize);
    assert(allocate_size % ALIGNMENT == 0);

    char *header = NULL;
    dbg_print_free_list();

    if ((intptr_t)(header = find_fit(allocate_size)) == -1)
    {
        if (expand(max(kChunkSize, allocate_size + kFreeListNodeSize)) == -1)
            return NULL;

        free_list_check();
        dbg_printf("\tEXPAND HEAP: size = %d\n", max(kChunkSize, allocate_size + kFreeListNodeSize));
        dbg_print_heap();

        header = find_fit(allocate_size);
        assert(!is_allocated(header));
    }

    heap_assert(header + 4);
    free_list_check();
    dbg_print_free_list();
    assert(!is_allocated(header));
    size_t origin_size = get_size(header);

    free_list_check();
    if (origin_size >= allocate_size + 2 * kHeaderSize + kFreeListNodeSize + kMinimumPayloadSize)
    {
        char *pre_free_block = prev_free_block(header);
        char *nxt_free_block = next_free_block(header);
        dbg_print_free_list();
        set_header(header, allocate_size, 1);
        set_footer(header, allocate_size, 1);

        // |h| ORIGIN |p|n|f|  -->  |h| PAYLOAD |pad|f|h|  NXT |p1|n1|f|
        // p1 and n1    NO CHANGE
        // update free_list_head if PAYLOAD.h == free_list_head

        char *next_header = next(header);
        set_header(next_header, origin_size - allocate_size, 0);
        set_footer(next_header, origin_size - allocate_size, 0);
        if (free_list_head == header)
        {
            assert(prev_free_block(next_header) == NULL);
            free_list_head = next_header;
            // set_prev_free_block(next_header, NULL);
        }

        if (free_list_tail == header)
        {
            assert(next_free_block(next_header) == NULL);
            free_list_tail = next_header;
            // set_next_free_block(next_header, NULL);
        }

        if (pre_free_block)
            set_next_free_block(pre_free_block, next_header);

        if (nxt_free_block)
            set_prev_free_block(nxt_free_block, next_header);

        free_list_assert(next_header);
        free_list_check();
        dbg_print_free_list();
    }
    else
    {
        remove_free_list_node(header);

        set_header(header, origin_size, 1);
        set_footer(header, origin_size, 1);

        free_list_check();
    }

    address_assert(header);
    address_assert(next(header));

    // if (header == first_find_header)
    // {
    //     while (!is_end(first_find_header) && is_allocated(first_find_header))
    //     {
    //         first_find_header = next(first_find_header);
    //         dbg_printf("\tMM_MALLOC NEXT SET FIRST_FIND_HEADER=%p, ORIGIN=%p\n", first_find_header, header);
    //     }
    //     dbg_printf("\tMM_MALLOC SET FIRST_FIND_HEADER=%p, ORIGIN=%p\n", first_find_header, header);
    // }
    // address_assert(header);
    // address_assert(first_find_header);

    dbg_printf("MALLOC: size=%u, allocate_size=%u, ptr=%p\n", size, allocate_size, header);
    dbg_print_heap();
    dbg_print_free_list();
    free_list_check();
    
    return header + 4;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    free_list_check();
    dbg_printf("FREE %p\n", ptr);
    dbg_print_heap();
    dbg_print_free_list();

    char *header = (char *)ptr - kHeaderSize;
    *header &= ~0x7;
    *get_footer(header) &= ~0x7;

    dbg_print_free_list();

    insert_free_list_node(header);

    dbg_print_free_list();
    free_list_check();
    collapse(header);

    dbg_print_heap();
    dbg_print_free_list();
    free_list_check();
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    dbg_printf("REALLOC: ptr=%p, size=%d\n", ptr, size);
    if (!ptr)
        return mm_malloc(size);

    if (!size)
    {
        mm_free(ptr);
        return NULL;
    }

    int origin_size = get_size((char *)ptr - kHeaderSize);
    int aligned_size = ALIGN(size);
    free_list_check();
    // | ORIGIN  | -/-> |h| ALIGNED_SIZE |p|n|f|h|p|payload|n|f|  ( |h|align|p|n|f| + |MFS| )
    if (aligned_size + kMinimumPayloadSize + 2 * kHeaderSize + kFreeListNodeSize > origin_size)
    {
        char *cur_header = (char *)ptr - kHeaderSize;
        char *origin_next_header = next(cur_header);

        address_assert(cur_header);
        address_assert(origin_next_header);

        int next_size = is_allocated(next(cur_header)) ? 0 : get_size(next(cur_header));
        assert(next_size % 8 == 0);

        // | cur block  |  next block(not allocated) |  -->  | expanded cur block |  next block |
        //  sizeof(expanded cur block) == aligned_size + 2 * header_size + free_list_node_size
        //  sizeof(next block) == 2 * header_size + free_list_node_size + payload_size
        //  sizeof(expanded cur block) + sizeof(next block) == origin_size + next_size
        //
        //  algined_size + 16 + 16 + payload_size == origin_size + next_size
        //  1. payload_size == origin_size + next_size - aligned_size - 32
        //  2. payload_size + 2 * header_size + free_list_node_size >= mininum_free_size
        //  2.a. payload_size + 16 >= MFS
        //  2.b. origin_size + next_size - aligned_size - 16 >= MFS
        if (next_size)
        {
            if (origin_size + next_size >= aligned_size + 16 + kMinimumPayloadSize)
            {
                dbg_print_free_list();

                remove_free_list_node(origin_next_header);
                *(uintptr_t *)origin_next_header |= 0x1;
                *get_footer(origin_next_header) |= 0x1;

                dbg_print_free_list();
                free_list_check();

                set_header(cur_header, aligned_size + 2 * kHeaderSize + kFreeListNodeSize, 1);
                set_footer(cur_header, aligned_size + 2 * kHeaderSize + kFreeListNodeSize, 1);

                char *next_header = next(cur_header);

                address_assert(next_header);

                set_header(next_header, origin_size + next_size - aligned_size - 16, 0);
                set_footer(next_header, origin_size + next_size - aligned_size - 16, 0);
                assert(get_size(next_header) + get_size(cur_header) == origin_size + next_size);
                
                insert_free_list_node(next_header);

                dbg_print_heap();
                dbg_print_free_list();
                free_list_check();
                heap_assert(cur_header + 4);

                return cur_header + 4;
            }
        }

        char *new_payload = mm_malloc(aligned_size);
        heap_assert(new_payload);
        address_assert(new_payload - kHeaderSize);

        if (!new_payload)
            return NULL;

        // memcpy(new_payload, (char *)ptr, get_size((char *)ptr - kHeaderSize) - 2 * kHeaderSize - kFreeListNodeSize);
        memcpy(new_payload, (char *)ptr, size);
        dbg_print_heap();
        dbg_print_free_list();

        mm_free(ptr);
        dbg_print_free_list();
        free_list_check();
        return new_payload;
    }
    // | ORIGIN  | --> |h| ALIGNED_SIZE |p|n|f|h|p|payload|n|f|  ( |h|align|p|n|f| + |MFS| )
    else
    {
        dbg_print_heap();
        dbg_print_free_list();

        char *header = (char *)ptr - 4;
        char *origin_next = next(header);

        set_header(header, aligned_size + 2 * kHeaderSize + kFreeListNodeSize, 1);
        set_footer(header, aligned_size + 2 * kHeaderSize + kFreeListNodeSize, 1);

        address_assert(header);

        header = next(header);
        set_header(header, origin_size - 16 - aligned_size, 0);
        set_footer(header, origin_size - 16 - aligned_size, 0);
        insert_free_list_node(header);

        dbg_print_free_list();
        free_list_check();
        address_assert(header);
        dbg_print_heap();


        heap_assert(ptr);
        return ptr;
    }
}
