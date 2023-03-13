/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
void transpose_32_32_opt(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, blk_row_ind, blk_col_ind;
    const int blk_size = 8;
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
                int temp = B[i][j];
                B[i][j] = B[i + blk_size][j + blk_size];
                B[i + blk_size][j + blk_size] = temp;
            }
}

void transpose_64_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, blk_row_ind, blk_col_ind;
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
        }

    for (blk_row_ind = 0; blk_row_ind < M / blk_size; blk_row_ind++)
        for (blk_col_ind = 0; blk_col_ind < N / blk_size; blk_col_ind += 2)
            for (i = blk_row_ind * blk_size + sub_blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                {
                    int temp = B[i][j];
                    B[i][j] = B[i][j + blk_size];
                    B[i][j + blk_size] = temp;
                }
}

void transpose_64_64_blk(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, blk_row_ind, blk_col_ind;
    const int blk_size = 4;
    for (blk_row_ind = 0; blk_row_ind < N / blk_size; blk_row_ind++)
    {
        for (blk_col_ind = 0; blk_col_ind < M / blk_size; blk_col_ind++)
        {
            if (blk_row_ind == blk_col_ind)
                continue;
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
            {
                for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                {
                    B[j][i] = A[i][j];
                }
            }
        }
    }

    for (blk_row_ind = 0; blk_row_ind < N / blk_size; blk_row_ind++)
    {
        if ((blk_row_ind / 2) % 2)
        {
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                    B[j - 2 * blk_size][i - 2 * blk_size] = A[i][j];
        }
        else
        {
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
                for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                    B[j + 2 * blk_size][i + 2 * blk_size] = A[i][j];
        }
    }

    for (blk_row_ind = 0; blk_row_ind < N / blk_size; blk_row_ind += 4)
    {
        for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
            for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
            {
                int temp = B[i][j];
                B[i][j] = B[i + 2 * blk_size][j + 2 * blk_size];
                B[i + 2 * blk_size][j + 2 * blk_size] = temp;
            }

        for (i = blk_row_ind * blk_size + blk_size; i < blk_row_ind * blk_size + 2 * blk_size; i++)
            for (j = blk_row_ind * blk_size + blk_size; j < blk_row_ind * blk_size + 2 * blk_size; j++)
            {
                int temp = B[i][j];
                B[i][j] = B[i + 2 * blk_size][j + 2 * blk_size];
                B[i + 2 * blk_size][j + 2 * blk_size] = temp;
            }
    }
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

            // print_matrix(M, N, B);
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

int get_set(void *addr)
{
    return (((long)addr) >> 5) & 0x1f;
}

void transpose_61_67(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    // int cnt = 0;
    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
        {
            // fprintf(stderr, "%d, %d\n", i, j);
            if (get_set(&A[i][j]) == get_set(&B[j][i]))
            {
                // cnt++;
                continue;
            }
            B[j][i] = A[i][j];
        }

    for (i = 0; i < N; i++)
        for (j = 0; j < M; j++)
        {
            // fprintf(stderr, "%d, %d\n", i, j);
            if (get_set(&A[i][j]) == get_set(&B[j][i]))
            {
                // fprintf(stderr, "%d, %d\n", i, j);
                B[j][i] = A[i][j];
            }
        }

    // fprintf(stderr, "%d\n", cnt);
}

void transpose_61_67_opt(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, blk_row_ind, blk_col_ind, divide_bound;
    const int blk_size = 6;
    divide_bound = M / blk_size * blk_size;
    if (N / blk_size * blk_size < divide_bound)
        divide_bound = N / blk_size * blk_size;
    for (blk_row_ind = 0; blk_row_ind < divide_bound / blk_size; blk_row_ind++)
    {
        for (blk_col_ind = 0; blk_col_ind < divide_bound / blk_size; blk_col_ind++)
        {
            for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
            {
                for (j = blk_col_ind * blk_size; j < blk_col_ind * blk_size + blk_size; j++)
                    B[j][i] = A[i][j];
            }
        }
    }

    for (blk_row_ind = 0; blk_row_ind < divide_bound / blk_size; blk_row_ind += 2)
        for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
            for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                B[j + blk_size][i + blk_size] = A[i][j];

    for (blk_row_ind = 1; blk_row_ind < divide_bound / blk_size; blk_row_ind += 2)
        for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
            for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
                B[j - blk_size][i - blk_size] = A[i][j];

    for (blk_row_ind = 0; blk_row_ind < divide_bound / blk_size; blk_row_ind += 2)
        for (i = blk_row_ind * blk_size; i < blk_row_ind * blk_size + blk_size; i++)
            for (j = blk_row_ind * blk_size; j < blk_row_ind * blk_size + blk_size; j++)
            {
                int temp = B[i][j];
                B[i][j] = B[i + blk_size][j + blk_size];
                B[i + blk_size][j + blk_size] = temp;
            }

    for (i = 0; i < divide_bound; i++)
        for (j = divide_bound; j < M; j++)
            B[j][i] = A[i][j];

    for (i = divide_bound; i < N; i++)
        for (j = 0; j < M; j++)
            B[j][i] = A[i][j];
}

void transpose_61_67_blk(int M, int N, int A[N][M], int B[M][N])
{
    const int blk_size = 23;
    for (int i = 0; i < N; i += blk_size)
        for (int j = 0; j < M; j += blk_size)
            for (int k = i; k < i + blk_size && k < N; k++)
                for (int l = j; l < j + blk_size && l < M; l++)
                    B[l][k] = A[k][l];
}

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    // (s, E, b) = (5, 1, 5)
    // 32 sets, 1 line per set, 32byte per block
    if (M == 32 && N == 32)
    {
        transpose_32_32_opt(M, N, A, B);
    }
    else if (M == 64 && N == 64)
    {
        transpose_64_64(M, N, A, B);
    }
    else
    {
        transpose_61_67_blk(M, N, A, B);
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    FILE *dbg_out = fopen("./trans.dbg.txt", "w+");
    fprintf(dbg_out, "%p, %p\n", A, B);

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
