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
static char *prologue_payload = NULL;
static char *epilogue_payload = NULL;
static const uint32_t kChunkSize = 4096u;
static const uint32_t kHeaderSize = 4u;

// |  header  |  payload  |  footer  |
// |    4     |     n     |    4     |
// size = 8 + n

// |    size   |   allocated-bit   |
// |    29     |         3         |
static inline uint32_t is_allocated(const char *payload);
static inline uint32_t get_size(const char *payload);
static inline uint32_t *get_header(const char *payload);
static inline uint32_t get_header_content(const char *payload);
static inline uint32_t get_footer_content(const char *payload);
static inline uint32_t *get_footer(const char *payload);
static inline void set_header(char *payload, const size_t size, const size_t is_allocated);
static inline void set_footer(char *payload, const size_t size, const size_t is_allocated);
static inline char *next(const char *payload);
static inline char *prev(const char *payload);
static inline int is_end(const char *payload);
static inline void erase_header(char *payload);
static inline void collapse(char *payload);
static inline char *find_first_fit(const size_t size);
static inline int expand(const size_t size);
static inline size_t max(size_t a, size_t b);
static inline void address_assert(void *payload_addr);

static inline uint32_t is_allocated(const char *payload)
{
    return get_header_content(payload) & 0x7;
}

static inline uint32_t get_size(const char *payload)
{
    return get_header_content(payload) & ~0x7;
}

static inline uint32_t *get_header(const char *payload)
{
    return (uint32_t *)(payload - kHeaderSize);
}

static inline uint32_t get_header_content(const char *payload)
{
    return *get_header(payload);
}

static inline uint32_t get_footer_content(const char *payload)
{
    return *get_footer(payload);
}

static inline uint32_t *get_footer(const char *payload)
{
    return (uint32_t *)(payload - 2 * kHeaderSize + get_size(payload));
}

static inline void set_header(char *payload, const size_t size, const size_t is_allocated)
{
    *get_header(payload) = size | is_allocated;
}

static inline void set_footer(char *payload, const size_t size, const size_t is_allocated)
{
    *get_footer(payload) = size | is_allocated;
}

static inline char *next(const char *payload)
{
    assert(get_size(payload) != 0);
    return (char *)payload + get_size(payload);
}

static inline char *prev(const char *payload)
{
    assert(get_size(payload) != 0);
    uint32_t prev_size = (*(uint32_t *)(payload - 8)) & ~0x7;
    return (char *)payload - prev_size;
}

static inline int is_end(const char *payload)
{
    return get_header_content(payload) == 0x1u;
}

static inline void erase_header(char *payload)
{
}

static inline void collapse(char *payload)
{
    char *pre = prev(payload);
    char *nxt = next(payload);

    int pre_allocated = is_allocated(pre);
    int nxt_allocated = is_allocated(nxt);

    int pre_size = get_size(pre);
    int cur_size = get_size(payload);
    int nxt_size = get_size(nxt);

    if (pre_allocated && nxt_allocated)
        ;
    else if (pre_allocated && !nxt_allocated)
    {
        *get_header(payload) += nxt_size;
        *get_footer(nxt) += cur_size;
    }
    else if (!pre_allocated && nxt_allocated)
    {
        *get_header(pre) += cur_size;
        *get_footer(payload) += pre_size;
    }
    else if (!pre_allocated && !nxt_allocated)
    {
        *get_header(pre) += cur_size + nxt_size;
        *get_footer(nxt) += cur_size + pre_size;
    }
}

static inline char *find_first_fit(const size_t size)
{
    char *cur = prologue_payload;
    while (!is_end(cur))
        if (!is_allocated(cur) && get_size(cur) >= size)
            break;
        else
            cur = next(cur);

    return is_end(cur) ? (char *)-1 : cur;
}

static inline int expand(const size_t size)
{
    if (mem_sbrk(size) == (void *)-1)
        return -1;

    set_header(epilogue_payload, size, 0);
    set_footer(epilogue_payload, size, 0);
    char *before_epi = epilogue_payload;
    epilogue_payload = next(epilogue_payload);
    set_header(epilogue_payload, 0, 1);

    collapse(before_epi);
    return 0;
}

static inline size_t max(size_t a, size_t b)
{
    return a < b ? b : a;
}

static inline void address_assert(void *payload_addr)
{
    assert(((uintptr_t)payload_addr) % ALIGNMENT == 0);
}

static inline void dbg_print_heap(void)
{
#ifdef DEBUG
    putchar('\t');
    char *cur = prologue_payload;
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

static inline void dbg_printf(const char *format, ...)
{
#ifdef DEBUG
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    fflush(NULL);
    va_end(args);
#endif
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

    prologue_payload = (char *)ALIGN(((uintptr_t)heap_start) + kHeaderSize);
    address_assert(prologue_payload);

    if ((intptr_t)mem_sbrk(prologue_payload - heap_start) == -1)
    {
        dbg_printf("\tMM_INIT FAILED\n");
        return -1;
    }

    set_header(prologue_payload, 2 * kHeaderSize, 1);
    set_footer(prologue_payload, 2 * kHeaderSize, 1);

    epilogue_payload = next(prologue_payload);
    address_assert(epilogue_payload);

    set_header(epilogue_payload, 0, 1);

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

    char *payload = NULL;

    if ((intptr_t)(payload = find_first_fit(allocate_size)) == -1)
    {
        if (expand(max(kChunkSize, allocate_size)) == -1)
            return NULL;
        dbg_printf("\tEXPAND HEAP: size = %d\n", max(kChunkSize, allocate_size));
        payload = find_first_fit(allocate_size);
    }

    size_t origin_size = get_size(payload);

    if (origin_size >= allocate_size + 2 * kHeaderSize)
    {
        set_header(payload, allocate_size, 1);
        set_footer(payload, allocate_size, 1);

        set_header(next(payload), origin_size - allocate_size, 0);
        set_footer(next(payload), origin_size - allocate_size, 0);
    }
    else
    {
        set_header(payload, origin_size, 1);
        set_footer(payload, origin_size, 1);
    }

    address_assert(payload);
    address_assert(next(payload));

    dbg_printf("MALLOC: size=%u, allocate_size=%u, ptr=%p\n", size, allocate_size, payload);
    dbg_print_heap();
    return payload;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    dbg_printf("FREE %p\n", ptr);
    dbg_print_heap();

    *get_header((char *)ptr) &= ~0x7;
    *get_footer((char *)ptr) &= ~0x7;
    collapse((char *)ptr);

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

    char *new_payload = mm_malloc(size);
    if (!new_payload)
        return NULL;

    memcpy(new_payload, (char *)ptr, get_size((char *)ptr) - 8);
    mm_free(ptr);

    return new_payload;
}
