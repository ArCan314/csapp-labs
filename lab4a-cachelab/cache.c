#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cache.h"

typedef struct AddrExtractPack
{
    unsigned long tag;
    unsigned long set;
    unsigned long byte;
} AddrExtractPack;

static long get_cache_set_size(Cache *cache);
// static long get_cache_line_length(Cache *cache);
// static long get_cache_data_size(Cache *cache);
static AddrExtractPack extract(Cache *cache, unsigned long addr);
static CacheLineInfo *get_info_by_tag(CacheLineInfo *info, unsigned long tag);
static CacheLineInfo *get_invalid_info(CacheLineInfo *info);
static void cache_load(Cache *cache, unsigned long tag, CacheLineLink *link, CacheManipResult *res);
static void insert_at_head(CacheLineLink *link, CacheLineInfo *info);
static void move_info_to_head(CacheLineLink *link, CacheLineInfo *info);

Cache *init_cache(int set_bit, int line_num, int byte_bit)
{

    Cache *res = (Cache *)malloc(sizeof(Cache));
    if (!res)
        return NULL;

    res->set_bit = set_bit;
    res->line_num = line_num;
    res->byte_bit = byte_bit;

    // res->data = (unsigned char *)malloc(sizeof(unsigned char) * get_cache_data_size(res));
    // if (!res->data)
    // {
    //     destroy_cache(res);
    //     return NULL;
    // }

#ifdef DEBUG
    printf("cache data size = %ld\n", get_cache_data_size(res));
    printf("cache set size = %ld\n", get_cache_set_size(res));
#endif

    res->infos = (CacheLineLink *)calloc(get_cache_set_size(res), sizeof(CacheLineLink));
    if (!res->infos)
    {
        destroy_cache(res);
        return NULL;
    }

#ifdef DEBUG
    printf("CACHE_LINK_CHECK:\n");
    for (int i = 0; i < get_cache_set_size(res); i++)
    {
        printf("(ind, head, tail, size) = (%d, %p, %p, %d)\n", i, res->infos[i].head, res->infos[i].tail, res->infos[i].size);
    }
#endif


    return res;
}

void destroy_cache(Cache *cache)
{
    if (!cache)
        return;

    if (cache->data)
        free(cache->data);

    if (cache->infos)
    {
        long info_size = get_cache_set_size(cache);
        for (long i = 0; i < info_size; i++)
        {
            CacheLineInfo *cur = cache->infos[i].head;
            while (cur)
            {
                CacheLineInfo *next = cur->next;
                free(cur);
                cur = next;
            }
        }
        free(cache->infos);
    }

    free(cache);
}

void cache_read(Cache *cache, unsigned long addr, CacheManipResult *res)
{
    AddrExtractPack ext_pack = extract(cache, addr);
#ifdef DEBUG
    printf("(tag, set, byte) = (%lx, %ld, %ld)\n", ext_pack.tag, ext_pack.set, ext_pack.byte);
#endif
    CacheLineLink *link = &(cache->infos[ext_pack.set]);

    CacheLineInfo *info = get_info_by_tag(link->head, ext_pack.tag);

    if (!info)
    {
        res->miss++;
        cache_load(cache, ext_pack.tag, link, res);
    }
    else
    {
        res->hit++;
        move_info_to_head(link, info);
    }
}

void cache_write(Cache *cache, unsigned long addr, CacheManipResult *res)
{
    cache_read(cache, addr, res);
}

void cache_modify(Cache *cache, unsigned long addr, CacheManipResult *res)
{
    AddrExtractPack ext_pack = extract(cache, addr);
#ifdef DEBUG
    printf("(tag, set, byte) = (%lx, %ld, %ld)\n", ext_pack.tag, ext_pack.set, ext_pack.byte);
#endif
    CacheLineLink *link = &(cache->infos[ext_pack.set]);

    CacheLineInfo *info = get_info_by_tag(link->head, ext_pack.tag);

    if (!info)
    {
        res->miss++;
        res->hit++;
        cache_load(cache, ext_pack.tag, link, res);
    }
    else
    {
        res->hit += 2;
        move_info_to_head(link, info);
    }
}

static long get_cache_set_size(Cache *cache)
{
    return (1 << cache->set_bit);
}

// static long get_cache_line_length(Cache *cache)
// {
//     return get_cache_set_size(cache) * cache->line_num;
// }

// static long get_cache_data_size(Cache *cache)
// {
//     return get_cache_line_length(cache) * (1 << cache->byte_bit);
// }

static AddrExtractPack extract(Cache *cache, unsigned long addr)
{
    unsigned long byte_mask = (1ul << cache->byte_bit) - 1ul;
    unsigned long set_mask = (1ul << cache->set_bit) - 1ul;
    // printf("%x, %x\n", addr >> cache->byte_bit, (addr >> cache->byte_bit) >> cache->set_bit);

    AddrExtractPack res;
    res.byte = addr & byte_mask;
    res.set = (addr >> cache->byte_bit) & set_mask;
    res.tag = ((addr >> cache->byte_bit) >> cache->set_bit);
    return res;
}

static CacheLineInfo *get_info_by_tag(CacheLineInfo *info, unsigned long tag)
{
    if (!info)
        return NULL;

    CacheLineInfo *cur = info;
    while (cur)
    {
        if (cur->tag == tag && cur->valid)
            break;
        cur = cur->next;
    }
    return cur;
}

static CacheLineInfo *get_invalid_info(CacheLineInfo *info)
{
    if (!info)
        return NULL;

    CacheLineInfo *cur = info;
    while (cur)
    {
        if (!cur->valid)
            break;
        cur = cur->next;
    }
    return cur;
}

static void cache_load(Cache *cache, unsigned long tag, CacheLineLink *link, CacheManipResult *res)
{
    if (link->size < cache->line_num)
    {
        CacheLineInfo *info = (CacheLineInfo *)calloc(1, sizeof(CacheLineInfo));
        info->valid = 1;
        info->tag = tag;

        insert_at_head(link, info);
    }
    else
    {
        CacheLineInfo *info = get_invalid_info(link->head);
        if (info)
        {
            info->valid = 1;
            info->tag = tag;
            move_info_to_head(link, info);
        }
        else
        {
            move_info_to_head(link, link->tail);
            link->head->tag = tag;
        }
        res->eviction++;
    }
}

static void insert_at_head(CacheLineLink *link, CacheLineInfo *info)
{
    info->next = link->head;
    info->pre = NULL;

    if (link->head)
        link->head->pre = info;
    else
        link->tail = info;
    link->head = info;
    link->size++;

#ifdef DEBUG
    // printf("CACHE_LINK_CHECK:\n");
    // printf("(head, tail, size) = (%p, %p, %d)\n", link->head, link->tail, link->size);
#endif
}

static void move_info_to_head(CacheLineLink *link, CacheLineInfo *info)
{
    if (!info || info == link->head)
        return;

    if (info == link->tail)
        link->tail = info->pre;
    else
        info->next->pre = info->pre;
    info->pre->next = info->next; // info != link->head


    link->head->pre = info;
    info->pre = NULL;
    info->next = link->head;
    link->head = info;

#ifdef DEBUG
    // printf("CACHE_LINK_CHECK:\n");
    // printf("(head, tail, size) = (%p, %p, %d)\n", link->head, link->tail, link->size);
#endif
}