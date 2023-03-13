#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef struct CacheLineInfo
{
    unsigned char valid;
    unsigned long tag;
    struct CacheLineInfo *next;
    struct CacheLineInfo *pre;
} CacheLineInfo;

typedef struct CacheLineLink
{
    CacheLineInfo *head;
    CacheLineInfo *tail;
    int size;
} CacheLineLink;

typedef struct Cache
{
    int set_bit;
    int line_num;
    int byte_bit;

    unsigned char *data;
    CacheLineLink *infos;
} Cache;

typedef struct CacheManipResult 
{
    int miss;
    int hit;
    int eviction;
} CacheManipResult;

Cache *init_cache(int set_bit, int line_num, int byte_bit);
void destroy_cache(Cache *cache);

void cache_read(Cache *cache, unsigned long addr, CacheManipResult *res);
void cache_write(Cache *cache, unsigned long addr, CacheManipResult *res);
void cache_modify(Cache *cache, unsigned long addr, CacheManipResult *res);

