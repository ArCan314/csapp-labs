#include <stdio.h>

// ROW = N; COL = M
#define ROW 64
#define COL 64

int A[ROW][COL], B[COL][ROW];
const char set_map[] =
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
     'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
     'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
     'U', 'V'};

void print_matrix(int r, int c, int m[r][c])
{
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
        printf("%d\t", i);
        for (int j = 0; j < c; j++)
        {
            if (j % 8 == 0)
                putchar('|');
            putchar((char)m[i][j]);
        }
        putchar('|');
        putchar('\n');
    }

    putchar('\n');
}

void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    // (s, E, b) = (5, 1, 5)
    // 32 sets, 1 line per set, 32byte per block

    int i, j, blk_row_ind, blk_col_ind;
    if (M == 32 && N == 32)
    {
        int blk_size = 8;
        for (blk_row_ind = 0; blk_row_ind < N / blk_size; blk_row_ind++)
        {
            for (blk_col_ind = 0; blk_col_ind < M / blk_size; blk_col_ind++)
            {
                if (blk_row_ind == blk_col_ind)
                    continue;

                for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                {
                    for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                        B[j][i] = A[i][j];
                }
            }
        }

        for (blk_row_ind = 0; blk_row_ind < N / blk_size; blk_row_ind += 2)
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                    B[j + blk_size][i + blk_size] = A[i][j];

        for (blk_row_ind = 1; blk_row_ind < N / blk_size; blk_row_ind += 2)
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                    B[j - blk_size][i - blk_size] = A[i][j];

        for (blk_row_ind = 0; blk_row_ind < N / blk_size; blk_row_ind += 2)
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                {
                    char temp = B[i][j];
                    B[i][j] = B[i + blk_size][j + blk_size];
                    B[i + blk_size][j + blk_size] = temp;
                }
    }
    else if (M == 64 && N == 64)
    {
        const int blk_size = 8, sub_blk_size = 4;
        for (blk_col_ind = 0; blk_col_ind < N / blk_size; blk_col_ind++)
            for (blk_row_ind = 0; blk_row_ind < M / blk_size; blk_row_ind++)
            {
                if (blk_row_ind % 2)
                {
                    for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + sub_blk_size; i++)
                        for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + sub_blk_size; j++)
                            B[j][i] = A[i][j];

                    for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + sub_blk_size; i++)
                        for (j = blk_col_ind * blk_size + sub_blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                            B[j][i - blk_size] = A[i][j];

                    for (i = blk_row_ind * blk_size + sub_blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                        for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + sub_blk_size; j++)
                            B[j][i] = A[i][j];

                    for (i = blk_row_ind * blk_size + sub_blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                        for (j = blk_col_ind * blk_size + sub_blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                            B[j][i - blk_size] = A[i][j];
                }
                else
                {
                    for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + sub_blk_size; i++)
                        for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + sub_blk_size; j++)
                            B[j][i] = A[i][j];

                    for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + sub_blk_size; i++)
                        for (j = blk_col_ind * blk_size + sub_blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                            B[j][i + blk_size] = A[i][j];

                    for (i = blk_row_ind * blk_size + sub_blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                        for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + sub_blk_size; j++)
                            B[j][i] = A[i][j];

                    for (i = blk_row_ind * blk_size + sub_blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                        for (j = blk_col_ind * blk_size + sub_blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                            B[j][i + blk_size] = A[i][j];
                }
                print_matrix(M, N, B);
            }

        for (blk_row_ind = 0; blk_row_ind < M / blk_size; blk_row_ind++)
            for (blk_col_ind = 0; blk_col_ind < N / blk_size; blk_col_ind += 2)
                for (i = blk_row_ind * blk_size + sub_blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                    for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                    {
                        char temp = B[i][j];
                        B[i][j] = B[i][j + blk_size];
                        B[i][j + blk_size] = temp;
                    }
    }
}

int get_set(long addr)
{
    return (((long)addr) >> 5) & 0x1f;
}
void transpose_64_64_opt(int M, int N, int A[N][M], int B[M][N])
{
    const int blk_size = 8, sub_blk_size = 4;
    for (int i = 0; i < N; i += blk_size)
        for (int j = 0; j < M; j += blk_size)
        {
            // printf("i=%d, j=%d\n", i, j);
            if (i == j)
                continue;

            for (int k = i; k < i + sub_blk_size; k++)
            {
                int l = j;
                int t1 = A[k][l];
                int t2 = A[k][l + 1];
                int t3 = A[k][l + 2];
                int t4 = A[k][l + 3];

                B[l][k] = t1;
                B[l + 1][k] = t2;
                B[l + 2][k] = t3;
                B[l + 3][k] = t4;
            }
            print_matrix(M, N, B);
            for (int k = i; k < i + sub_blk_size; k++)
            {
                int l = j + sub_blk_size;
                int t1 = A[k][l];
                int t2 = A[k][l + 1];
                int t3 = A[k][l + 2];
                int t4 = A[k][l + 3];

                // printf("l - sub_blk_size=%d, k=%d\n", l - sub_blk_size, k);
                B[l - sub_blk_size][k + sub_blk_size] = t1;
                B[l + 1 - sub_blk_size][k + sub_blk_size] = t2;
                B[l + 2 - sub_blk_size][k + sub_blk_size] = t3;
                B[l + 3 - sub_blk_size][k + sub_blk_size] = t4;
            }
            print_matrix(M, N, B);
            for (int k = i + sub_blk_size; k < i + blk_size; k++)
            {
                int l = j;
                int t1 = A[k][l];
                int t2 = A[k][l + 1];
                int t3 = A[k][l + 2];
                int t4 = A[k][l + 3];

                B[l + sub_blk_size][k - sub_blk_size] = t1;
                B[l + 1 + sub_blk_size][k - sub_blk_size] = t2;
                B[l + 2 + sub_blk_size][k - sub_blk_size] = t3;
                B[l + 3 + sub_blk_size][k - sub_blk_size] = t4;
            }
            print_matrix(M, N, B);
            for (int k = i + sub_blk_size; k < i + blk_size; k++)
            {
                int l = j + sub_blk_size;
                int t1 = A[k][l];
                int t2 = A[k][l + 1];
                int t3 = A[k][l + 2];
                int t4 = A[k][l + 3];

                B[l][k] = t1;
                B[l + 1][k] = t2;
                B[l + 2][k] = t3;
                B[l + 3][k] = t4;
            }

            print_matrix(M, N, B);
        }

    for (int i = 0; i < N; i += blk_size)
        for (int j = 0; j < M; j += blk_size)
        {
            if (i == j)
                continue;

            for (int k = i; k < i + sub_blk_size; k++)
            {
                int l = j + sub_blk_size;
                int t1 = B[k][l];
                int t2 = B[k][l + 1];
                int t3 = B[k][l + 2];
                int t4 = B[k][l + 3];

                int t5 = B[k + sub_blk_size][l - sub_blk_size];
                int t6 = B[k + sub_blk_size][l - sub_blk_size + 1];
                int t7 = B[k + sub_blk_size][l - sub_blk_size + 2];
                int t8 = B[k + sub_blk_size][l - sub_blk_size + 3];

                B[k + sub_blk_size][l - sub_blk_size] = t1;
                B[k + sub_blk_size][l - sub_blk_size + 1] = t2;
                B[k + sub_blk_size][l - sub_blk_size + 2] = t3;
                B[k + sub_blk_size][l - sub_blk_size + 3] = t4;

                B[k][l] = t5;
                B[k][l + 1] = t6;
                B[k][l + 2] = t7;
                B[k][l + 3] = t8;
            }
        }

    for (int i = 0; i < N; i += blk_size)
    {
        for (int k = i; k < i + blk_size; k++)
        {
            int t1 = A[k][i];
            int t2 = A[k][i + 1];
            int t3 = A[k][i + 2];
            int t4 = A[k][i + 3];
            int t5 = A[k][i + 4];
            int t6 = A[k][i + 5];
            int t7 = A[k][i + 6];
            int t8 = A[k][i + 7];

            B[i][k] = t1;
            B[i + 1][k] = t2;
            B[i + 2][k] = t3;
            B[i + 3][k] = t4;
            B[i + 4][k] = t5;
            B[i + 5][k] = t6;
            B[i + 6][k] = t7;
            B[i + 7][k] = t8;
        }
    }
}
void transpose_61_67_opt(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    int cnt = 0;
    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
        {
            if (get_set((long)A + (i * M + j) * 4) == get_set((long)B + (j * N + i) * 4))
            {
                cnt++;
                continue;
            }
            B[j][i] = A[i][j];
        }

    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
        {

            if (get_set((long)A + (i * M + j) * 4) == get_set((long)B + (j * N + i) * 4))
            {
                fprintf(stderr, "%d, %d\n", i, j);
                B[j][i] = A[i][j];
            }
        }

    fprintf(stderr, "CNT=%d\n", cnt);
}
int main(void)
{
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            A[i][j] = set_map[((((i * COL) + j) * 4) >> 5) & 0x1f];

    const int N = ROW, M = COL;

    printf("Matrix A\n");
    print_matrix(N, M, A);

    // transpose_submit(r, c, A, B);
    transpose_64_64_opt(M, N, A, B);

    printf("Matrix B\n");
    print_matrix(M, N, B);

    putchar('\n');
    return 0;
}