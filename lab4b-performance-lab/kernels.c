/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "bdsfadsfovik", /* Team name */

    "Harry Q.dasf Bovik",     /* First member full name */
    "b`ovik@nowhedasfre.edu", /* First member email address */

    "dasf ", /* Second member full name (leave blank if none) */
    "asdf "  /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst)
{
    int fwd_ind = 0, bwd_ind_init = dim - 1;
    for (int i = 0; i < dim; i++)
    {
        int bwd_ind = bwd_ind_init--;

        for (int j = 0; j < dim; j++)
        {
            dst[fwd_ind++] = src[bwd_ind];
            // memcpy(dst + bwd_ind, src + fwd_ind, sizeof(pixel));

            bwd_ind += dim;
            // fwd_ind += 1;
        }
    }
}

char rotate_expand_descr[] = "rotate: expanded";
void rotate_expand(int dim, pixel *src, pixel *dst)
{
    int fwd_ind = 0, bwd_ind_init = dim - 1, dim2 = dim * 16;
    for (int i = 0; i < dim; i++)
    {
        int bwd_ind = bwd_ind_init--;

        for (int j = 0; j < dim; j += 16)
        {
            dst[fwd_ind] = src[bwd_ind];
            dst[fwd_ind + 1] = src[bwd_ind + dim];
            dst[fwd_ind + 2] = src[bwd_ind + 2 * dim];
            dst[fwd_ind + 3] = src[bwd_ind + 3 * dim];
            dst[fwd_ind + 4] = src[bwd_ind + 4 * dim];
            dst[fwd_ind + 5] = src[bwd_ind + 5 * dim];
            dst[fwd_ind + 6] = src[bwd_ind + 6 * dim];
            dst[fwd_ind + 7] = src[bwd_ind + 7 * dim];
            dst[fwd_ind + 8] = src[bwd_ind + 8 * dim];
            dst[fwd_ind + 9] = src[bwd_ind + 9 * dim];
            dst[fwd_ind + 10] = src[bwd_ind + 10 * dim];
            dst[fwd_ind + 11] = src[bwd_ind + 11 * dim];
            dst[fwd_ind + 12] = src[bwd_ind + 12 * dim];
            dst[fwd_ind + 13] = src[bwd_ind + 13 * dim];
            dst[fwd_ind + 14] = src[bwd_ind + 14 * dim];
            dst[fwd_ind + 15] = src[bwd_ind + 15 * dim];
            // dst[fwd_ind + 16] = src[bwd_ind + 16 * dim];
            // dst[fwd_ind + 17] = src[bwd_ind + 17 * dim];
            // dst[fwd_ind + 18] = src[bwd_ind + 18 * dim];
            // dst[fwd_ind + 19] = src[bwd_ind + 19 * dim];
            // dst[fwd_ind + 20] = src[bwd_ind + 20 * dim];
            // dst[fwd_ind + 21] = src[bwd_ind + 21 * dim];
            // dst[fwd_ind + 22] = src[bwd_ind + 22 * dim];
            // dst[fwd_ind + 23] = src[bwd_ind + 23 * dim];
            // dst[fwd_ind + 24] = src[bwd_ind + 24 * dim];
            // dst[fwd_ind + 25] = src[bwd_ind + 25 * dim];
            // dst[fwd_ind + 26] = src[bwd_ind + 26 * dim];
            // dst[fwd_ind + 27] = src[bwd_ind + 27 * dim];
            // dst[fwd_ind + 28] = src[bwd_ind + 28 * dim];
            // dst[fwd_ind + 29] = src[bwd_ind + 29 * dim];
            // dst[fwd_ind + 30] = src[bwd_ind + 30 * dim];
            // dst[fwd_ind + 31] = src[bwd_ind + 31 * dim];

            // memcpy(dst + bwd_ind, src + fwd_ind, sizeof(pixel));

            bwd_ind += dim2;
            fwd_ind += 16;
            // fwd_ind += 1;
        }
    }
}

// for (j = 0; j < dim; j++)
// dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
// (dim - 1 - j) * dim + i
// dim * dim - dim - j * dim + i
//
// i * dim + j
/*
    1 2 3 4
    5 6 7 8
    9 0 a b
    c d e f
    
    4 8 b f
    3 7 a e
    2 6 0 d 
    1 5 9 c
    */

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions()
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    add_rotate_function(&rotate_expand, rotate_expand_descr);
    /* ... Register additional test functions here */
}

/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct
{
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum)
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p)
{
    sum->red += (int)p.red;
    sum->green += (int)p.green;
    sum->blue += (int)p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short)(sum.red / sum.num);
    current_pixel->green = (unsigned short)(sum.green / sum.num);
    current_pixel->blue = (unsigned short)(sum.blue / sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) // read 9dim^2 + 6dim - 23 write dim^2
{
    for (int i = 1; i < dim - 1; i++)
        for (int j = 1; j < dim - 1; j++)
        {
            int r = 0, g = 0, b = 0;
            for (int row = i - 1; row <= i + 1; row++)
                for (int col = j - 1; col <= j + 1; col++)
                {
                    r += src[row * dim + col].red;
                    g += src[row * dim + col].green;
                    b += src[row * dim + col].blue;
                }
            pixel temp = {r / 9, g / 9, b / 9};
            dst[i * dim + j] = temp;
        }

    {
        int r, g, b;
        r = src[0].red + src[1].red + src[dim].red + src[dim + 1].red;
        g = src[0].green + src[1].green + src[dim].green + src[dim + 1].green;
        b = src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[0] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[i - 1].red + src[i].red + src[i + 1].red + src[dim + i - 1].red + src[dim + i].red + src[dim + i + 1].red;
        g = src[i - 1].green + src[i].green + src[i + 1].green + src[dim + i - 1].green + src[dim + i].green + src[dim + i + 1].green;
        b = src[i - 1].blue + src[i].blue + src[i + 1].blue + src[dim + i - 1].blue + src[dim + i].blue + src[dim + i + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[i] = temp;
    }

    {
        int r, g, b;
        r = src[dim - 2].red + src[dim - 1].red + src[2 * dim - 2].red + src[2 * dim - 1].red;
        g = src[dim - 2].green + src[dim - 1].green + src[2 * dim - 2].green + src[2 * dim - 1].green;
        b = src[dim - 2].blue + src[dim - 1].blue + src[2 * dim - 2].blue + src[2 * dim - 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[dim - 1] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[(dim - 2) * dim + i - 1].red + src[(dim - 2) * dim + i].red + src[(dim - 2) * dim + i + 1].red + src[(dim - 1) * dim + i - 1].red + src[(dim - 1) * dim + i].red + src[(dim - 1) * dim + i + 1].red;
        g = src[(dim - 2) * dim + i - 1].green + src[(dim - 2) * dim + i].green + src[(dim - 2) * dim + i + 1].green + src[(dim - 1) * dim + i - 1].green + src[(dim - 1) * dim + i].green + src[(dim - 1) * dim + i + 1].green;
        b = src[(dim - 2) * dim + i - 1].blue + src[(dim - 2) * dim + i].blue + src[(dim - 2) * dim + i + 1].blue + src[(dim - 1) * dim + i - 1].blue + src[(dim - 1) * dim + i].blue + src[(dim - 1) * dim + i + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[(dim - 1) * dim + i] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[(i - 1) * dim].red + src[(i - 1) * dim + 1].red + src[(i)*dim].red + src[(i)*dim + 1].red + src[(i + 1) * dim].red + src[(i + 1) * dim + 1].red;
        g = src[(i - 1) * dim].green + src[(i - 1) * dim + 1].green + src[(i)*dim].green + src[(i)*dim + 1].green + src[(i + 1) * dim].green + src[(i + 1) * dim + 1].green;
        b = src[(i - 1) * dim].blue + src[(i - 1) * dim + 1].blue + src[(i)*dim].blue + src[(i)*dim + 1].blue + src[(i + 1) * dim].blue + src[(i + 1) * dim + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[i * dim] = temp;
    }

    {
        int r, g, b;
        r = src[(dim - 2) * dim].red + src[(dim - 1) * dim].red + src[(dim - 2) * dim + 1].red + src[(dim - 1) * dim + 1].red;
        g = src[(dim - 2) * dim].green + src[(dim - 1) * dim].green + src[(dim - 2) * dim + 1].green + src[(dim - 1) * dim + 1].green;
        b = src[(dim - 2) * dim].blue + src[(dim - 1) * dim].blue + src[(dim - 2) * dim + 1].blue + src[(dim - 1) * dim + 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[(dim - 1) * dim] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[(i - 1) * dim + dim - 2].red + src[(i - 1) * dim + dim - 1].red + src[(i)*dim + dim - 1].red + src[(i)*dim + dim - 2].red + src[(i + 1) * dim + dim - 2].red + src[(i + 1) * dim + dim - 1].red;
        g = src[(i - 1) * dim + dim - 2].green + src[(i - 1) * dim + dim - 1].green + src[(i)*dim + dim - 1].green + src[(i)*dim + dim - 2].green + src[(i + 1) * dim + dim - 2].green + src[(i + 1) * dim + dim - 1].green;
        b = src[(i - 1) * dim + dim - 2].blue + src[(i - 1) * dim + dim - 1].blue + src[(i)*dim + dim - 1].blue + src[(i)*dim + dim - 2].blue + src[(i + 1) * dim + dim - 2].blue + src[(i + 1) * dim + dim - 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[i * dim + dim - 1] = temp;
    }

    {
        int r, g, b;
        r = src[(dim - 2) * dim + dim - 2].red + src[(dim - 2) * dim + dim - 1].red + src[(dim - 1) * dim + dim - 2].red + src[(dim - 1) * dim + dim - 1].red;
        g = src[(dim - 2) * dim + dim - 2].green + src[(dim - 2) * dim + dim - 1].green + src[(dim - 1) * dim + dim - 2].green + src[(dim - 1) * dim + dim - 1].green;
        b = src[(dim - 2) * dim + dim - 2].blue + src[(dim - 2) * dim + dim - 1].blue + src[(dim - 1) * dim + dim - 2].blue + src[(dim - 1) * dim + dim - 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[dim * dim - 1] = temp;
    }
}

char smooth_opt_descr[] = "smooth_opt: smooth optimization";
void smooth_opt(int dim, pixel *src, pixel *dst) // read 9dim^2 + 6dim - 23 write dim^2
{
    for (int i = 1; i < dim - 1; i++)
        for (int j = 1; j < dim - 1; j++)
        {
            int r = 0, g = 0, b = 0;
            for (int row = i - 1; row <= i + 1; row++)
                for (int col = j - 1; col <= j + 1; col++)
                {
                    r += src[row * dim + col].red;
                    g += src[row * dim + col].green;
                    b += src[row * dim + col].blue;
                }
            pixel temp = {r / 9, g / 9, b / 9};
            dst[i * dim + j] = temp;
        }

    {
        int r, g, b;
        r = src[0].red + src[1].red + src[dim].red + src[dim + 1].red;
        g = src[0].green + src[1].green + src[dim].green + src[dim + 1].green;
        b = src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[0] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[i - 1].red + src[i].red + src[i + 1].red + src[dim + i - 1].red + src[dim + i].red + src[dim + i + 1].red;
        g = src[i - 1].green + src[i].green + src[i + 1].green + src[dim + i - 1].green + src[dim + i].green + src[dim + i + 1].green;
        b = src[i - 1].blue + src[i].blue + src[i + 1].blue + src[dim + i - 1].blue + src[dim + i].blue + src[dim + i + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[i] = temp;
    }

    {
        int r, g, b;
        r = src[dim - 2].red + src[dim - 1].red + src[2 * dim - 2].red + src[2 * dim - 1].red;
        g = src[dim - 2].green + src[dim - 1].green + src[2 * dim - 2].green + src[2 * dim - 1].green;
        b = src[dim - 2].blue + src[dim - 1].blue + src[2 * dim - 2].blue + src[2 * dim - 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[dim - 1] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[(dim - 2) * dim + i - 1].red + src[(dim - 2) * dim + i].red + src[(dim - 2) * dim + i + 1].red + src[(dim - 1) * dim + i - 1].red + src[(dim - 1) * dim + i].red + src[(dim - 1) * dim + i + 1].red;
        g = src[(dim - 2) * dim + i - 1].green + src[(dim - 2) * dim + i].green + src[(dim - 2) * dim + i + 1].green + src[(dim - 1) * dim + i - 1].green + src[(dim - 1) * dim + i].green + src[(dim - 1) * dim + i + 1].green;
        b = src[(dim - 2) * dim + i - 1].blue + src[(dim - 2) * dim + i].blue + src[(dim - 2) * dim + i + 1].blue + src[(dim - 1) * dim + i - 1].blue + src[(dim - 1) * dim + i].blue + src[(dim - 1) * dim + i + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[(dim - 1) * dim + i] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[(i - 1) * dim].red + src[(i - 1) * dim + 1].red + src[(i)*dim].red + src[(i)*dim + 1].red + src[(i + 1) * dim].red + src[(i + 1) * dim + 1].red;
        g = src[(i - 1) * dim].green + src[(i - 1) * dim + 1].green + src[(i)*dim].green + src[(i)*dim + 1].green + src[(i + 1) * dim].green + src[(i + 1) * dim + 1].green;
        b = src[(i - 1) * dim].blue + src[(i - 1) * dim + 1].blue + src[(i)*dim].blue + src[(i)*dim + 1].blue + src[(i + 1) * dim].blue + src[(i + 1) * dim + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[i * dim] = temp;
    }

    {
        int r, g, b;
        r = src[(dim - 2) * dim].red + src[(dim - 1) * dim].red + src[(dim - 2) * dim + 1].red + src[(dim - 1) * dim + 1].red;
        g = src[(dim - 2) * dim].green + src[(dim - 1) * dim].green + src[(dim - 2) * dim + 1].green + src[(dim - 1) * dim + 1].green;
        b = src[(dim - 2) * dim].blue + src[(dim - 1) * dim].blue + src[(dim - 2) * dim + 1].blue + src[(dim - 1) * dim + 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[(dim - 1) * dim] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = src[(i - 1) * dim + dim - 2].red + src[(i - 1) * dim + dim - 1].red + src[(i)*dim + dim - 1].red + src[(i)*dim + dim - 2].red + src[(i + 1) * dim + dim - 2].red + src[(i + 1) * dim + dim - 1].red;
        g = src[(i - 1) * dim + dim - 2].green + src[(i - 1) * dim + dim - 1].green + src[(i)*dim + dim - 1].green + src[(i)*dim + dim - 2].green + src[(i + 1) * dim + dim - 2].green + src[(i + 1) * dim + dim - 1].green;
        b = src[(i - 1) * dim + dim - 2].blue + src[(i - 1) * dim + dim - 1].blue + src[(i)*dim + dim - 1].blue + src[(i)*dim + dim - 2].blue + src[(i + 1) * dim + dim - 2].blue + src[(i + 1) * dim + dim - 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        dst[i * dim + dim - 1] = temp;
    }

    {
        int r, g, b;
        r = src[(dim - 2) * dim + dim - 2].red + src[(dim - 2) * dim + dim - 1].red + src[(dim - 1) * dim + dim - 2].red + src[(dim - 1) * dim + dim - 1].red;
        g = src[(dim - 2) * dim + dim - 2].green + src[(dim - 2) * dim + dim - 1].green + src[(dim - 1) * dim + dim - 2].green + src[(dim - 1) * dim + dim - 1].green;
        b = src[(dim - 2) * dim + dim - 2].blue + src[(dim - 2) * dim + dim - 1].blue + src[(dim - 1) * dim + dim - 2].blue + src[(dim - 1) * dim + dim - 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        dst[dim * dim - 1] = temp;
    }
}

typedef struct
{
    int red;
    int green;
    int blue;
} pixel_sump;

char smooth_prepare_descr[] = "smooth: prefix sum 1d";
void smooth_prepare(int dim, pixel *src, pixel *dst) //
{
    pixel *td = dst;
    pixel_sump *sdst = malloc(sizeof(pixel_sump) * dim * dim);

    for (int i = 0; i < dim; i++)
    {
        sdst[i * dim].red = src[i * dim].red;
        sdst[i * dim].green = src[i * dim].green;
        sdst[i * dim].blue = src[i * dim].blue;
    }

    for (int i = 0; i < dim; i++)
        for (int j = 1; j < dim; j++)
        {
            sdst[i * dim + j].red = sdst[i * dim + j - 1].red + src[i * dim + j].red;
            sdst[i * dim + j].green = sdst[i * dim + j - 1].green + src[i * dim + j].green;
            sdst[i * dim + j].blue = sdst[i * dim + j - 1].blue + src[i * dim + j].blue;
        }

    for (int i = 1; i < dim - 1; i++)
        for (int j = 2; j < dim - 1; j++)
        {
            int r, g, b;
            r = sdst[(i - 1) * dim + j + 1].red - sdst[(i - 1) * dim + j - 2].red + sdst[i * dim + j + 1].red - sdst[i * dim + j - 2].red + sdst[(i + 1) * dim + j + 1].red - sdst[(i + 1) * dim + j - 2].red;
            g = sdst[(i - 1) * dim + j + 1].green - sdst[(i - 1) * dim + j - 2].green + sdst[i * dim + j + 1].green - sdst[i * dim + j - 2].green + sdst[(i + 1) * dim + j + 1].green - sdst[(i + 1) * dim + j - 2].green;
            b = sdst[(i - 1) * dim + j + 1].blue - sdst[(i - 1) * dim + j - 2].blue + sdst[i * dim + j + 1].blue - sdst[i * dim + j - 2].blue + sdst[(i + 1) * dim + j + 1].blue - sdst[(i + 1) * dim + j - 2].blue;
            pixel temp = {r / 9, g / 9, b / 9};
            td[i * dim + j] = temp;
        }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = sdst[(i - 1) * dim + 2].red + sdst[(i)*dim + 2].red + sdst[(i + 1) * dim + 2].red;
        g = sdst[(i - 1) * dim + 2].green + sdst[(i)*dim + 2].green + sdst[(i + 1) * dim + 2].green;
        b = sdst[(i - 1) * dim + 2].blue + sdst[(i)*dim + 2].blue + sdst[(i + 1) * dim + 2].blue;
        pixel temp = {r / 9, g / 9, b / 9};
        td[i * dim + 1] = temp;
    }

    {
        int r, g, b;
        r = sdst[1].red + sdst[1 + dim].red;
        g = sdst[1].green + sdst[1 + dim].green;
        b = sdst[1].blue + sdst[1 + dim].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        td[0] = temp;
    }

    {
        int r, g, b;
        r = sdst[2].red + sdst[2 + dim].red;
        g = sdst[2].green + sdst[2 + dim].green;
        b = sdst[2].blue + sdst[2 + dim].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        td[1] = temp;
    }

    {
        int r, g, b;
        r = sdst[(dim - 1) * dim + 1].red + sdst[(dim - 2) * dim + 1].red;
        g = sdst[(dim - 1) * dim + 1].green + sdst[(dim - 2) * dim + 1].green;
        b = sdst[(dim - 1) * dim + 1].blue + sdst[(dim - 2) * dim + 1].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        td[(dim - 1) * dim] = temp;
    }

    {
        int r, g, b;
        r = sdst[(dim - 1) * dim + 2].red + sdst[(dim - 2) * dim + 2].red;
        g = sdst[(dim - 1) * dim + 2].green + sdst[(dim - 2) * dim + 2].green;
        b = sdst[(dim - 1) * dim + 2].blue + sdst[(dim - 2) * dim + 2].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        td[(dim - 1) * dim + 1] = temp;
    }

    {
        int r, g, b;
        r = sdst[dim - 1].red - sdst[dim - 3].red + sdst[2 * dim - 1].red - sdst[2 * dim - 3].red;
        g = sdst[dim - 1].green - sdst[dim - 3].green + sdst[2 * dim - 1].green - sdst[2 * dim - 3].green;
        b = sdst[dim - 1].blue - sdst[dim - 3].blue + sdst[2 * dim - 1].blue - sdst[2 * dim - 3].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        td[dim - 1] = temp;
    }

    {
        int r, g, b;
        r = sdst[(dim - 1) * dim + dim - 1].red - sdst[(dim - 1) * dim + dim - 3].red + sdst[(dim - 2) * dim + dim - 1].red - sdst[(dim - 2) * dim + dim - 3].red;
        g = sdst[(dim - 1) * dim + dim - 1].green - sdst[(dim - 1) * dim + dim - 3].green + sdst[(dim - 2) * dim + dim - 1].green - sdst[(dim - 2) * dim + dim - 3].green;
        b = sdst[(dim - 1) * dim + dim - 1].blue - sdst[(dim - 1) * dim + dim - 3].blue + sdst[(dim - 2) * dim + dim - 1].blue - sdst[(dim - 2) * dim + dim - 3].blue;
        pixel temp = {r / 4, g / 4, b / 4};
        td[dim * dim - 1] = temp;
    }

    for (int i = 2; i < dim - 1; i++)
    {
        int r, g, b;
        r = sdst[i + 1].red - sdst[i - 2].red + sdst[dim + i + 1].red - sdst[dim + i - 2].red;
        g = sdst[i + 1].green - sdst[i - 2].green + sdst[dim + i + 1].green - sdst[dim + i - 2].green;
        b = sdst[i + 1].blue - sdst[i - 2].blue + sdst[dim + i + 1].blue - sdst[dim + i - 2].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        td[i] = temp;
    }

    for (int i = 2; i < dim - 1; i++)
    {
        int r, g, b;
        r = sdst[(dim - 1) * dim + i + 1].red - sdst[(dim - 1) * dim + i - 2].red + sdst[(dim - 2) * dim + i + 1].red - sdst[(dim - 2) * dim + i - 2].red;
        g = sdst[(dim - 1) * dim + i + 1].green - sdst[(dim - 1) * dim + i - 2].green + sdst[(dim - 2) * dim + i + 1].green - sdst[(dim - 2) * dim + i - 2].green;
        b = sdst[(dim - 1) * dim + i + 1].blue - sdst[(dim - 1) * dim + i - 2].blue + sdst[(dim - 2) * dim + i + 1].blue - sdst[(dim - 2) * dim + i - 2].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        td[(dim - 1) * dim + i] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = sdst[(i - 1) * dim + 1].red + sdst[(i)*dim + 1].red + sdst[(i + 1) * dim + 1].red;
        g = sdst[(i - 1) * dim + 1].green + sdst[(i)*dim + 1].green + sdst[(i + 1) * dim + 1].green;
        b = sdst[(i - 1) * dim + 1].blue + sdst[(i)*dim + 1].blue + sdst[(i + 1) * dim + 1].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        td[i * dim] = temp;
    }

    for (int i = 1; i < dim - 1; i++)
    {
        int r, g, b;
        r = sdst[(i - 1) * dim + dim - 1].red + sdst[(i)*dim + dim - 1].red + sdst[(i + 1) * dim + dim - 1].red - sdst[(i - 1) * dim + dim - 3].red - sdst[(i)*dim + dim - 3].red - sdst[(i + 1) * dim + dim - 3].red;
        g = sdst[(i - 1) * dim + dim - 1].green + sdst[(i)*dim + dim - 1].green + sdst[(i + 1) * dim + dim - 1].green - sdst[(i - 1) * dim + dim - 3].green - sdst[(i)*dim + dim - 3].green - sdst[(i + 1) * dim + dim - 3].green;
        b = sdst[(i - 1) * dim + dim - 1].blue + sdst[(i)*dim + dim - 1].blue + sdst[(i + 1) * dim + dim - 1].blue - sdst[(i - 1) * dim + dim - 3].blue - sdst[(i)*dim + dim - 3].blue - sdst[(i + 1) * dim + dim - 3].blue;
        pixel temp = {r / 6, g / 6, b / 6};
        td[i * dim + dim - 1] = temp;
    }

    free(sdst);
}

/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions()
{
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    add_smooth_function(&smooth_prepare, smooth_prepare_descr);
    /* ... Register additional test functions here */
}

/*
eax dim * -sizeof(pixel)
ebx -sizeof(pixel)
ecx src
edx increase
edi 
*/
