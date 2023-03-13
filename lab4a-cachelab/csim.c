#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "cache.h"
#include "cachelab.h"

// static void show_unknown_flag(char flag);
// static void show_help();

static const int kTracePathBufLen = 100 + 1;
static const int kReadBufSize = 100 + 1;

int main(int argc, char *const *argv)
{
    int opt;
    int verbose = 0;
    int set_bit = -1, block_bit = -1, line_count = 1;
    char trace_path[kTracePathBufLen];
    int trace_len = 0;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            // show_help();
            return 0;
            break;
        case 'v':
            verbose = 1;
            break;
        case 's':
            set_bit = atoi(optarg);
            break;
        case 'E':
            line_count = atoi(optarg);
            break;
        case 'b':
            block_bit = atoi(optarg);
            break;
        case 't':
        {
            const char *temp = optarg;
            int i;
            for (i = 0; (i < kTracePathBufLen - 1) && (*temp); i++)
                if (!isblank(*temp))
                {
                    trace_path[trace_len++] = *temp;
                    temp++;
                }
            trace_path[trace_len] = '\0';
        }
        break;
        default:
            // show_unknown_flag(*optarg);
            // show_help();
            return 0;
            break;
        }
    }

#ifdef DEBUG
    printf("(s,E,b) = (%d,%d,%d)\n", set_bit, line_count, block_bit);
#endif

    CacheManipResult res = {0, 0, 0};
    Cache *cache = init_cache(set_bit, line_count, block_bit);
    if (!cache)
    {
        // error msg
        return 0;
    }

    FILE *trace = fopen(trace_path, "r");
    if (!trace)
    {
        // error msg
        return 0;
    }

    char buf[kReadBufSize];
    while (fgets(buf, sizeof(buf), trace) != NULL)
    {
        if (buf[0] != ' ')
            continue;
        char op;
        unsigned long addr;
        sscanf(buf, " %c %lx", &op, &addr);
#ifdef DEBUG
        // printf("parse op=%c, addr=%x\n", op, addr);
#endif
        CacheManipResult before = res;

        if (op == 'S')
            cache_read(cache, addr, &res);
        else if (op == 'L')
            cache_write(cache, addr, &res);
        else
            cache_modify(cache, addr, &res);

        if (verbose)
        {
            for (int i = 1; i < strlen(buf) - 1; i++)
                putchar(buf[i]);
            putchar(' ');

            for (int i = before.miss; i < res.miss; i++)
                printf("miss ");

            for (int i = before.eviction; i < res.eviction; i++)
                printf("eviction ");

            for (int i = before.hit; i < res.hit; i++)
                printf("hit ");
            putchar('\n');
        }
    }

    printSummary(res.hit, res.miss, res.eviction);
    destroy_cache(cache);
    return 0;
}
