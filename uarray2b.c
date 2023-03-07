/**************************************************************
 *                     uarray2b.c
 *     Assignment: HW3 locality
 *     Authors:  Arjun Kantamsetty (akanta01) and Vir Bhatia (vbhati02)
 *     Date:     February 17, 2023
 *   
 *     implementation of the blocked array
 *     
 **************************************************************/

#include "uarray.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define T UArray2b_T

static int UArray2b_blkheight(T array2b);
static int UArray2b_blkwidth(T array2b);

const int blocksize_64KB = 65536;

struct T {
        int width;
        int height;
        int size;
        int blocksize;
        int blkheight;
        int blkwidth;
        UArray2_T block_arr;
};

/**********UArray2b_new********
 * Creates a new UArray2b_T object with given parameters
 * Inputs:
 *              int height: number of rows in the 2D array
 *              int width: number of cols in the 2D array
 *              int size: number of bytes occupied by an element in the 
 *                        UArray2_T object
 *              int blocksize: length of one dimension of a block
 * Return: UArray2b_T object
 * Expects: all parameters to be positive         
 ************************/
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        /* Ensures proper parameters are passed in */
        assert(blocksize > 0);
        assert(width > 0);
        assert(height > 0);
        assert(size > 0);
        /* Allocate space for struct */
        T barray = malloc(sizeof(*barray));
        assert(barray != NULL);

        /* Initialize struct values */
        barray -> width = width;
        barray -> height = height;
        barray -> size = size;
        barray -> blocksize = blocksize;

        /* Gets number of blocks wide */
        barray -> blkwidth = width / blocksize;
        if (width % blocksize != 0)
        {
                (barray -> blkwidth)++;
        }
        /* Gets number of blocks high */
        barray -> blkheight = height / blocksize;
        if (height % blocksize != 0)
        {
                (barray -> blkheight)++;
        }
        
        UArray_T dummy;
        barray -> block_arr = UArray2_new(barray->blkwidth, barray->blkheight,
                        (sizeof(dummy)));

        /* Generates each block (puts UArrays in each block) */
        for (int i = 0; i < barray->blkheight; i++) {
                for (int j = 0; j < barray->blkwidth; j++) {
                        UArray_T *block;

                        block = UArray2_at(barray -> block_arr, j, i);
                        *block = UArray_new(blocksize * blocksize,
                                (barray->size));
                }
        }
        return barray;
}

/**********UArray2b_new_64K_block********
 * Creates a new UArray2b_T object with given parameters, which defaults a
 * blocksize that is as large as possible while still allowing a block to fit
 * in 64KB of RAM
 * Inputs:
 *              int height: number of rows in the 2D array
 *              int width: number of cols in the 2D array
 *              int size: number of bytes occupied by an element in the 
 *                        UArray2_T object
 * Return: UArray2b_T object
 * Expects: all parameters to be positive         
 ************************/
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        if (size > blocksize_64KB) {
                return UArray2b_new(width, height, size, 1);
        }
        else {
                /* In case can fit in 64KB memory */
                int blocksize = sqrt(blocksize_64KB / size);
                return UArray2b_new(width, height, size, blocksize);
        }
}

/**********UArray2b_free********
 * frees the UArray2b_T object
 * Inputs: a pointer to the UArray2b_T
 * Return: N/A
 * Expects: *UArray2b and UArray2b should be non NULL
 *              
 ************************/
void UArray2b_free(T *array2b)
{
        /* Ensures uarray2b and the pointer to uarray2b is not NULL */
        assert(array2b != NULL);
        assert(*array2b != NULL);

        UArray_T *block;
        /* Loops through blocked array and frees each block */
        for (int i = 0; i < UArray2b_blkheight(*array2b); i++) {
                for (int j = 0; j < UArray2b_blkwidth(*array2b); j++) {
                        block = UArray2_at((*array2b) -> block_arr, j, i);
                        UArray_free(block);
                }
        }

        UArray2_free(&(*array2b)->block_arr);
        free(*array2b);
}

/**********UArray2b_width********
 * Returns the width of the UArray2b
 * Input: UArray2b struct object
 * Return: integer that represents the width/number of cols of the UArray2b
 * Expects: array2b is not null
 *      
 ************************/
int UArray2b_width(T array2b)
{
        assert(array2b != NULL);
        return array2b -> width;
}

/**********UArray2b_height********
 * Returns the height of the UArray2b
 * Inputs: UArray2 struct object
 * Return: integer that represent the height/number of rows of the UArray2b
 * Expects: array2b is not null
 *      
 ************************/
int UArray2b_height(T array2b)
{
        assert(array2b != NULL);
        return array2b -> height;
}

/**********UArray2b_size********
 * Returns the number of bytes occupied by an element in the UArray2b
 * Inputs: UArray2b struct object
 * Return: integer that represents the number of bytes occupied by an element
 *         in the UArray2b
 * Expects: array2b is not null
 *      
 ************************/
int UArray2b_size(T array2b)
{
        assert(array2b != NULL);
        return array2b -> size;
}

/**********UArray2b_blocksize********
 * Returns the length of one dimension of a block within the UArray2b
 * Inputs: UArray2 struct object
 * Return: integer that represents the length of one dimension of a block
 *         within the UArray2b
 * Expects: array2b is not null
 *      
 ************************/
int UArray2b_blocksize(T array2b)
{
        assert(array2b != NULL);
        return array2b -> blocksize;
}

/**********UArray2b_blkheight********
 *
 * Returns the number of blocks in a column within the UArray2b
 * Inputs:
 *         T array2b - A struct containing information about the UArray2b
                       parameters.
 * Return: A static integer containing the number of blocks in a column within
           the UArray2b.

 * Expects: Expects the array2b input struct to not be null.
 *              
 ************************/
static int UArray2b_blkheight(T array2b)
{
        assert(array2b != NULL);
        return array2b -> blkheight;
}

/**********UArray2b_blkwidth********
 *
 * Returns the number of blocks in a row within the UArray2b
 * Inputs:
 *         T array2b - A struct containing information about the UArray2b
                       parameters.
 * Return: A static integer containing the number of blocks in a row within
           the UArray2b.

 * Expects: Expects the array2b input struct to not be null.
 *              
 ************************/
static int UArray2b_blkwidth(T array2b)
{
        assert(array2b != NULL);
        return array2b -> blkwidth;
}

/**********UArray2b_at********
 * Returns pointer to the element in the cell specified by col and row
 * Inputs:
 *      UArray2b struct object and integers representing column and row
 *      of the cell called on
 * Return: pointer to the cell
 * Expects: col and row have to correspond to a valid position in a valid
 *          UArray2
 *      
 ************************/
void *UArray2b_at(T array2b, int column, int row)
{
        assert(array2b != NULL);
        /* Raises checked runtime error if given out of bounds indices */
        assert(column >= 0);
        assert(column < UArray2b_width(array2b));
        assert(row >= 0);
        assert(row < UArray2b_height(array2b));

        int blksize = UArray2b_blocksize(array2b);

        /* Gets index of the block */
        int blk_col = column / blksize;
        int blk_row = row / blksize;

        UArray_T *blk = UArray2_at(array2b -> block_arr, blk_col, blk_row);

        /* Gets index in 1D representation */
        int index = blksize * (column % blksize) + (row % blksize);

        assert(*blk != NULL);
        assert(blk != NULL);
        
        assert(UArray_at(*blk, index) != NULL);
        
        return UArray_at(*blk, index);
}

/**********UArray2b_map********
 * Traverses the array in block-major order and calls the apply function on
 * each element
 * Inputs:
 *              UArray2b_T struct obj, an apply function with its
 *              own parameters, and a void pointer cl to mark the end of the
 *              call.
 * Return: N/A
 * Expects: UArray2 object is passed in is not NULL
 *      
 ************************/
void UArray2b_map(T array2b, void apply(int col, int row, T array2b, 
        void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
        int blksize = UArray2b_blocksize(array2b);
        (void) cl;

        /* Loops through the blocked array */
        for (int i = 0; i < UArray2b_blkheight(array2b); i++) {
                for (int j = 0; j < UArray2b_blkwidth(array2b); j++) {
                        /* Loops through the block */
                        for (int k = 0; k < (blksize * blksize); k++) {
                                int col_idx = (blksize * j) + (k % blksize);
                                int row_idx = (blksize * i) + (k / blksize);
                                if (row_idx < UArray2b_height(array2b)) {
                                        if (col_idx < UArray2b_width(array2b)) {
                                        void *elem = UArray2b_at(array2b, 
                                                col_idx, row_idx);
                                        apply(col_idx, row_idx, array2b, elem, 
                                                cl);
                                        }
                                }       
                        }
                }
        }
}