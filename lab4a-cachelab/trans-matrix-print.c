#include <stdio.h>

const int row[] = {32, 64, 67};
const int col[] = {32, 64, 61};
const char set_map[] =
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
     'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
     'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
     'U', 'V'};

static int get_set(int addr)
{
    return (addr >> 5) & 0x1f;
}

int main(void)
{
    for (int ind = 0; ind < sizeof(row) / sizeof(int); ind++)
    {
        int r = row[ind], c = col[ind];
        printf("Matrix Size = %d X %d\n", r, c);

        printf("\t");
        for (int i = 0; i < c; i++)
        {
            if (i % 8 == 0)
                putchar('|');
            putchar((i % 10 == 0) ? '0' + i / 10 : ' ');
        }
        putchar('|');
        putchar('\n');

        printf("\t");
        for (int i = 0; i < c; i++)
        {
            if (i % 8 == 0)
                putchar('|');
            putchar(set_map[i % 10]);
        }
        putchar('|');
        putchar('\n');

        for (int i = 0; i < r; i++)
        {
            printf("%2d\t", i);
            for (int j = 0; j < c; j++)
            {
                if (j % 8 == 0)
                    putchar('|');
                putchar(set_map[get_set((i * r + j) * 4)]);
            }
            putchar('|');
            putchar('\n');
        }

        putchar('\n');
    }

    return 0;
}