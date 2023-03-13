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
static char *next_find_header = NULL;
static const uint32_t kChunkSize = 1024u * 4u;
static const uint32_t kHeaderSize = 4u;

// |  header  |  payload  |  footer  | ALLOCATED BLOCK
// |    4     |     n     |    4     |
// size = 8 + n

// |    size   |   allocated-bit   |
// |    29     |         3         |
static uint32_t is_allocated(const char *header);
static uint32_t get_size(const char *header);
// static uint32_t get_footer_content(const char *header);
static uint32_t *get_footer(const char *header);
static void set_header(char *header, const size_t size, const size_t is_allocated);
static void set_footer(char *header, const size_t size, const size_t is_allocated);
static char *next(const char *header);
static char *prev(const char *header);
static int is_end(const char *header);
// static void erase_header(char *header);
static void collapse(char *header);
static char *find_next_fit(const size_t size);
static int expand(const size_t size);
static size_t max(size_t a, size_t b);
static void address_assert(void *payload_addr);
static void *ptr_min(void *a, void *b);
static void dbg_printf(const char *format, ...);

static uint32_t is_allocated(const char *header)
{
    return *(uint32_t *)header & 0x7;
}

static uint32_t get_size(const char *header)
{
    return *(uint32_t *)header & ~0x7;
}

static uint32_t *get_footer(const char *header)
{
    return (uint32_t *)(header - kHeaderSize + get_size(header));
}

static void set_header(char *header, const size_t size, const size_t is_allocated)
{
    *((uint32_t *)header) = size | is_allocated;
}

static void set_footer(char *header, const size_t size, const size_t is_allocated)
{
    *get_footer(header) = size | is_allocated;
}

static char *next(const char *header)
{
    assert(get_size(header) != 0);
    return (char *)header + get_size(header);
}

static char *prev(const char *header)
{
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

    if (pre_allocated && nxt_allocated)
        ;
    else if (pre_allocated && !nxt_allocated)
    {
        (*(uint32_t *)header) += nxt_size;
        *get_footer(nxt) += cur_size;
    }
    else if (!pre_allocated && nxt_allocated)
    {
        (*(uint32_t *)pre) += cur_size;
        *get_footer(header) += pre_size;
    }
    else if (!pre_allocated && !nxt_allocated)
    {
        (*(uint32_t *)pre) += cur_size + nxt_size;
        *get_footer(nxt) += cur_size + pre_size;
    }

    if (!pre_allocated && header == next_find_header)
    {
        next_find_header = pre;
    }
}

static char *find_next_fit(const size_t size)
{
    char *cur = next_find_header;
    while (!is_end(cur))
        if (!is_allocated(cur) && get_size(cur) >= size)
            break;
        else
            cur = next(cur);

    if (!is_end(cur) && !is_allocated(cur) && get_size(cur) >= size)
        return cur;

    cur = prologue_header;
    while (cur != next_find_header)
        if (!is_allocated(cur) && get_size(cur) >= size)
            break;
        else
            cur = next(cur);

    if (cur != next_find_header)
    {
        next_find_header = cur;
        return cur;
    }
    return (char *)-1;
}

static int expand(const size_t size)
{
    if (mem_sbrk(size) == (void *)-1)
        return -1;

    set_header(epilogue_header, size, 0);
    set_footer(epilogue_header, size, 0);
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

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    dbg_printf("MM_INIT CALLED\n");
    assert(sizeof(uintptr_t) == sizeof(void *));
    assert(sizeof(uint32_t) == 4);

    heap_start = (char *)mem_sbrk(3 * kHeaderSize);
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

    set_header(prologue_header, 2 * kHeaderSize, 1);
    set_footer(prologue_header, 2 * kHeaderSize, 1);

    epilogue_header = next(prologue_header);
    address_assert(epilogue_header);

    set_header(epilogue_header, 0, 1);
    next_find_header = prologue_header;
    dbg_printf("\tMMINIT SET FIRST_FIND_HEADER=%p\n", next_find_header);

    dbg_print_heap();
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t allocate_size = ALIGN(size + 2 * kHeaderSize);
    assert(allocate_size % ALIGNMENT == 0);

    char *header = NULL;

    if ((intptr_t)(header = find_next_fit(allocate_size)) == -1)
    {
        if (expand(max(kChunkSize, allocate_size)) == -1)
            return NULL;
        dbg_printf("\tEXPAND HEAP: size = %d\n", max(kChunkSize, allocate_size));
        dbg_print_heap();
        header = find_next_fit(allocate_size);
    }

    size_t origin_size = get_size(header);

    if (origin_size >= allocate_size + 2 * kHeaderSize)
    {
        set_header(header, allocate_size, 1);
        set_footer(header, allocate_size, 1);

        set_header(next(header), origin_size - allocate_size, 0);
        set_footer(next(header), origin_size - allocate_size, 0);
    }
    else
    {
        set_header(header, origin_size, 1);
        set_footer(header, origin_size, 1);
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
    return header + 4;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    dbg_printf("FREE %p\n", ptr);
    dbg_print_heap();

    *((char *)ptr - 4) &= ~0x7;
    *get_footer((char *)ptr - 4) &= ~0x7;
    collapse((char *)ptr - 4);

    dbg_print_heap();
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (!ptr)
        return mm_malloc(size);

    if (!size)
    {
        mm_free(ptr);
        return NULL;
    }

    int origin_size = get_size((char *)ptr - kHeaderSize);
    int aligned_size = ALIGN(size);
    if (aligned_size > origin_size - 16)
    {
        char *cur_header = (char *)ptr - 4;
        char *origin_next_header = next(cur_header);

        address_assert(cur_header);
        address_assert(origin_next_header);

        int next_size = is_allocated(next(cur_header)) ? 0 : get_size(next(cur_header));
        assert(next_size % 8 == 0);

        // | cur block  |  next block(not allocated) | |h|p|f|h|pp|f| 16 + aligned_size + x == o + n
        //  origin_size + next_size >= aligned_size + 8
        if (next_size)
        {
            if (aligned_size + 16 <= origin_size + next_size)
            {
                set_header(cur_header, aligned_size + 8, 1);
                set_footer(cur_header, aligned_size + 8, 1);

                char *next_header = next(cur_header);

                address_assert(next_header);

                set_header(next_header, origin_size + next_size - aligned_size - 8, 0);
                set_footer(next_header, origin_size + next_size - aligned_size - 8, 0);
                
                if (origin_next_header == next_find_header)
                    next_find_header = next_header;

                return cur_header + 4;
            }
            else if (aligned_size + 8 <= origin_size + next_size)
            {
                set_header(cur_header, origin_size + next_size, 1);
                set_footer(cur_header, origin_size + next_size, 1);

                if (next_find_header == origin_next_header)
                    next_find_header = cur_header;

                return cur_header + 4;
            }
        }

        char *new_payload = mm_malloc(size);
        address_assert(new_payload - 4);

        if (!new_payload)
            return NULL;

        memcpy(new_payload, (char *)ptr, get_size((char *)ptr - 4) - 8);
        mm_free(ptr);
        return new_payload;
    }
    else if (origin_size - 16 - aligned_size >= 8)
    {
        dbg_print_heap();

        char *header = (char *)ptr - 4;
        char *origin_next = next(header);

        set_header(header, aligned_size + 8, 1);
        set_footer(header, aligned_size + 8, 1);

        address_assert(header);

        header = next(header);
        set_header(header, origin_size - 8 - aligned_size, 0);
        set_footer(header, origin_size - 8 - aligned_size, 0);

        address_assert(header);
        dbg_print_heap();

        if (origin_next == next_find_header)
            next_find_header = header;

        return ptr;
    }
    else
        return ptr;
}
