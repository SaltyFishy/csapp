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
    "go sleep when difficulties come",              /* Team name */

    "xyl",     /* First member full name */
    "",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
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
    int i, j, k, l, t;
    int temp = dim - 1;
    for (i = 0; i < dim; i += 8){
	for (j = 0; j < dim; j += 8){
	    for (l = j;l < j + 8;l++){
	        t = (temp - l) * dim;
	        for (k = i;k < i + 8;k++){
	            dst[t + k] = src[RIDX(k,l,dim)];
	        }
	    }
	}
    }
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    naive_rotate(dim, src, dst);
}

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
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
inline static int min(int a, int b) { return (a < b ? a : b); }
inline static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
inline static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
inline static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
inline static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
inline static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
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
void naive_smooth(int dim, pixel* src, pixel* dst)
{
    int i, j, temp;
    //left up
    temp = 0;
    dst[temp].red = (src[temp].red + src[1].red + src[dim].red + src[dim + 1].red) / 4;
    dst[temp].green = (src[temp].green + src[1].green + src[dim].green + src[dim + 1].green) / 4;
    dst[temp].blue = (src[temp].blue + src[1].blue + src[dim].blue + src[dim + 1].blue) / 4;
    //right up
    temp = dim - 1;
    dst[temp].red = (src[temp].red + src[dim - 2].red + src[2 * dim - 1].red + src[2 * dim - 2].red) / 4;
    dst[temp].green = (src[temp].green + src[dim - 2].green + src[2 * dim - 1].green + src[2 * dim - 2].green) / 4;
    dst[temp].blue = (src[temp].blue + src[dim - 2].blue + src[2 * dim - 1].blue + src[2 * dim - 2].blue) / 4;
    //left down
    temp = dim * (dim - 1);
    dst[temp].red = (src[temp].red + src[temp + 1].red + src[temp - dim].red + src[temp - dim + 1].red) / 4;
    dst[temp].green = (src[temp].green + src[temp + 1].green + src[temp - dim].green + src[temp - dim + 1].green) / 4;
    dst[temp].blue = (src[temp].blue + src[temp + 1].blue + src[temp - dim].blue + src[temp - dim + 1].blue) / 4;
    //right down
    temp = dim * dim;
    dst[temp - 1].red = (src[temp - 1].red + src[temp - 2].red + src[temp - dim - 1].red + src[temp - dim - 2].red) / 4;
    dst[temp - 1].green = (src[temp - 1].green + src[temp - 2].green + src[temp - dim - 1].green + src[temp - dim - 2].green) / 4;
    dst[temp - 1].blue = (src[temp - 1].blue + src[temp - 2].blue + src[temp - dim - 1].blue + src[temp - dim - 2].blue) / 4;
    //up
    for (i = 1; i < dim - 1; i++) {
        dst[i].red = (src[i - 1].red + src[i].red + src[i + 1].red + src[i + dim - 1].red + src[i + dim].red + src[i + 1 + dim].red) / 6;
        dst[i].green = (src[i - 1].green + src[i].green + src[i + 1].green + src[i + dim - 1].green + src[i + dim].green + src[i + 1 + dim].green) / 6;
        dst[i].blue = (src[i - 1].blue + src[i].blue + src[i + 1].blue + src[i + dim - 1].blue + src[i + dim].blue + src[i + 1 + dim].blue) / 6;
    }
    //left
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, 0,dim)].red = (src[RIDX(i, 0, dim)].red + src[RIDX(i, -dim, dim)].red + src[RIDX(i, dim, dim)].red + src[RIDX(i, 1, dim)].red + src[RIDX(i, -dim, dim) + 1].red + src[RIDX(i, dim, dim) + 1].red) / 6;
        dst[RIDX(i, 0,dim)].green = (src[RIDX(i, 0, dim)].green + src[RIDX(i, -dim, dim)].green + src[RIDX(i, dim, dim)].green + src[RIDX(i, 1, dim)].green + src[RIDX(i, -dim, dim) + 1].green + src[RIDX(i, dim, dim) + 1].green) / 6;
        dst[RIDX(i, 0, dim)].blue = (src[RIDX(i, 0, dim)].blue + src[RIDX(i, -dim, dim)].blue + src[RIDX(i, dim, dim)].blue + src[RIDX(i, 1, dim)].blue + src[RIDX(i, -dim, dim) + 1].blue + src[RIDX(i, dim, dim) + 1].blue) / 6;
    }
    //right
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, dim - 1,dim)].red = (src[RIDX(i, dim - 1, dim)].red + src[RIDX(i - 1, dim - 1, dim)].red + src[RIDX(i + 1, dim - 1, dim)].red + src[RIDX(i, dim - 2, dim)].red + src[RIDX(i - 1, dim - 2, dim)].red + src[RIDX(i + 1, dim - 2, dim)].red) / 6;
        dst[RIDX(i, dim - 1, dim)].green = (src[RIDX(i, dim - 1, dim)].green + src[RIDX(i - 1, dim - 1, dim)].green + src[RIDX(i + 1, dim - 1, dim)].green + src[RIDX(i, dim - 2, dim)].green + src[RIDX(i - 1, dim - 2, dim)].green + src[RIDX(i + 1, dim - 2, dim)].green) / 6;
        dst[RIDX(i, dim - 1, dim)].blue = (src[RIDX(i, dim - 1, dim)].blue + src[RIDX(i - 1, dim - 1, dim)].blue + src[RIDX(i + 1, dim - 1, dim)].blue + src[RIDX(i, dim - 2, dim)].blue + src[RIDX(i - 1, dim - 2, dim)].blue + src[RIDX(i + 1, dim - 2, dim)].blue) / 6;
    }
    //down
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(dim - 1,i,dim)].red = (src[RIDX(dim - 1, i, dim)].red + src[RIDX(dim - 1, i - 1, dim)].red + src[RIDX(dim - 1, i + 1, dim)].red + src[RIDX(dim - 2, i, dim)].red + src[RIDX(dim - 2, i - 1, dim)].red + src[RIDX(dim - 2, i + 1, dim)].red) / 6;
        dst[RIDX(dim - 1, i, dim)].green = (src[RIDX(dim - 1, i, dim)].green + src[RIDX(dim - 1, i - 1, dim)].green + src[RIDX(dim - 1, i + 1, dim)].green + src[RIDX(dim - 2, i, dim)].green + src[RIDX(dim - 2, i - 1, dim)].green + src[RIDX(dim - 2, i + 1, dim)].green) / 6;
        dst[RIDX(dim - 1, i, dim)].blue = (src[RIDX(dim - 1, i, dim)].blue + src[RIDX(dim - 1, i - 1, dim)].blue + src[RIDX(dim - 1, i + 1, dim)].blue + src[RIDX(dim - 2, i, dim)].blue + src[RIDX(dim - 2, i - 1, dim)].blue + src[RIDX(dim - 2, i + 1, dim)].blue) / 6;
    }
    //middle
    for (i = 1; i <= dim - 2;i++) {
        for (j = 1; j <= dim - 2;j++) {
            dst[RIDX(i,j,dim)].red = (src[RIDX(i, j, dim)].red + src[RIDX(i, j - 1, dim)].red + src[RIDX(i, j + 1, dim)].red + src[RIDX(i - 1, j, dim)].red + src[RIDX(i - 1, j - 1, dim)].red + src[RIDX(i - 1, j + 1, dim)].red + src[RIDX(i + 1, j, dim)].red + src[RIDX(i + 1, j - 1, dim)].red + src[RIDX(i + 1, j + 1, dim)].red) / 9;
            dst[RIDX(i,j,dim)].green = (src[RIDX(i, j, dim)].green + src[RIDX(i, j - 1, dim)].green + src[RIDX(i, j + 1, dim)].green + src[RIDX(i - 1, j, dim)].green + src[RIDX(i - 1, j - 1, dim)].green + src[RIDX(i - 1, j + 1, dim)].green + src[RIDX(i + 1, j, dim)].green + src[RIDX(i + 1, j - 1, dim)].green + src[RIDX(i + 1, j + 1, dim)].green) / 9;
            dst[RIDX(i,j,dim)].blue = (src[RIDX(i, j, dim)].blue + src[RIDX(i, j - 1, dim)].blue + src[RIDX(i, j + 1, dim)].blue + src[RIDX(i - 1, j, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue + src[RIDX(i + 1, j, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue + src[RIDX(i + 1, j + 1, dim)].blue) / 9;
        }
    }
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    naive_smooth(dim, src, dst);
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

