/**************************************************************
 *                     ppmtrans.c
 *     Assignment: HW3 locality
 *     Authors:  Arjun Kantamsetty (akanta01) and Vir Bhatia (vbhati02)
 *     Date:     February 23, 2023
 *   
 *     implementation of image transformation program
 *     
 **************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

struct closure {
        A2Methods_UArray2 raster;
        A2Methods_T arrayfxns;
};

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)


static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

/**********trans_ppm********
 *
 * calls rotation functions and measures the time taken to complete them
 * Inputs:
 *              Pnm_ppm orig_img: original image read in Pnm.h
 *              A2Methods_T methods: method suite of function pointers
 *              A2Methods_mapfun: map function which will traverse the image 
 *                                raster
 *              int angle: angle of rotation to be performed on image
 *              bool timer: boolean which is true if timing flag is given
 * Return:      time taken to complete the rotation
 * Expects:
 *              more than 1 command line argument to be supplied
 * Notes:
 *              assert exists to check if insufficient args are inputted
************************/
double trans_ppm(Pnm_ppm orig_img, A2Methods_T methods, A2Methods_mapfun map, 
        int angle, bool timer);

/**********cl_maker********
 *
 * Assigns required values to closure struct for transformation mapping
 * Inputs:
 *              int width: The desired width of the rotated image
 *              int height: The desired height of the rotated image
 *              struct closure *cl: The closure struct being updated
 *              A2Methods_T methods: Methods suite containing A2 
 *                                   manipulation functions
 * Return:      n/a
 * Expects:     n/a
 * Notes:
************************/
void cl_maker(int width, int height, struct closure *cl, A2Methods_T methods);

/**********rotate90********
 *
 * apply function to do 90 degree image rotation
 * Inputs:
 *              int col: column index of a cell
 *              int row: row index of a cell
 *              A2Methods_UArray2 arr: UArray2b object which will hold the
                                       updated image raster
                void *elem: A pointer to the element at index (col, row)
                void *cl_trans: pointer to the closure struct
                
 * Return:      n/a
 * Expects:
 *              more than 1 command line argument to be supplied
 * Notes:
 *              assert exists to check if insufficient args are inputted
************************/
void rotate90(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans);

/**********rotate180********
 *
 * apply function to do 180 degree image rotation
 * Inputs:
 *              int col: column index of a cell
 *              int row: row index of a cell
 *              A2Methods_UArray2 arr: UArray2b object which will hold the
                                       updated image raster
                void *elem: A pointer to the element at index (col, row)
                void *cl_trans: pointer to the closure struct
                
 * Return:      n/a
 * Expects:
 *              more than 1 command line argument to be supplied
 * Notes:
 *              assert exists to check if insufficient args are inputted
************************/
void rotate180(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans);

/**********rotate270********
 *
 * apply function to do 270 degree image rotation
 * Inputs:
 *              int col: column index of a cell
 *              int row: row index of a cell
 *              A2Methods_UArray2 arr: UArray2b object which will hold the
                                       updated image raster
                void *elem: A pointer to the element at index (col, row)
                void *cl_trans: pointer to the closure struct
                
 * Return:      n/a
 * Expects:
 *              more than 1 command line argument to be supplied
 * Notes:
 *              assert exists to check if insufficient args are inputted
************************/
void rotate270(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans);

/**********rotate0********
 *
 * apply function to do 0 degree image rotation
 * Inputs:
 *              int col: column index of a cell
 *              int row: row index of a cell
 *              A2Methods_UArray2 arr: UArray2b object which will hold the
                                       updated image raster
                void *elem: A pointer to the element at index (col, row)
                void *cl_trans: pointer to the closure struct
                
 * Return:      n/a
 * Expects:
 *              more than 1 command line argument to be supplied
 * Notes:
 *              assert exists to check if insufficient args are inputted
************************/
void rotate0(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans);

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;
        bool time_included = false;

        
        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);
        
        /* Defaults file to stdin */
        FILE *filename = stdin;
        FILE *time_fptr;
        
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                        /* Call functions for rotation here */

                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                        time_included = true;
                        /* Opens timing file */
                        time_fptr = fopen(time_file_name, "a");
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        filename = fopen(argv[i], "rb");
                        /* Assert to check if NULL */
                        if (filename == NULL) {
                                fprintf(stderr, "File not able to be opened\n");
                        }
                        if (time_included) {
                                fprintf(time_fptr, "Filename: %s\n", argv[i]);
                        }
                }
        }
        /* Reads in data into the Pnm_ppm obj */
        Pnm_ppm pixmap = Pnm_ppmread(filename, methods);
        double time_result = trans_ppm(pixmap, methods, map, rotation, 
                time_included);
        int num_pixels = pixmap->width * pixmap->height;
        
        /* Writes timing data to timing file */
        if (time_included) {
                fprintf(time_fptr, "Transformation: %d\n", rotation);
                fprintf(time_fptr, "Number of Pixels %d\n", num_pixels);
                fprintf(time_fptr, "Time taken to complete complete image" 
                        "transformation: %.0f\n", time_result);
                fprintf(time_fptr, "Time taken to complete transformation per" 
                        "pixel: %.0f\n", time_result / num_pixels);
        }
        fclose(filename);
        if (time_included) {
                fclose(time_fptr);
        }
        Pnm_ppmfree(&pixmap);

        exit(EXIT_SUCCESS);
}

double trans_ppm(Pnm_ppm orig_img, A2Methods_T methods, A2Methods_mapfun map, 
        int angle, bool time)
{
        struct closure *cl_trans = malloc(sizeof(struct closure));
        
        CPUTime_T clock = CPUTime_New();
        double elapsed_time = 0.0;

        /* Does nothing, writes to stdout */
        if (angle == 0) {
                cl_maker(orig_img->width, orig_img->height, cl_trans, methods);
                /* Starts the timing of the rotation */
                if (time) {
                        CPUTime_Start(clock);
                }
                (*map)(orig_img->pixels, (A2Methods_applyfun*) 
                                rotate0, cl_trans);
                if (time) {
                        elapsed_time = CPUTime_Stop(clock);
                }
        }
        /* Swaps height and width values */
        if ((angle == 90) || (angle == 270)) {
                cl_maker(orig_img->height, orig_img->width, cl_trans, methods);

                if (angle == 90) {
                        /* Starts the timing of the rotation */
                        if (time) {
                                CPUTime_Start(clock);
                                (*map)(orig_img->pixels, (A2Methods_applyfun*) 
                                        rotate90, cl_trans);
                                elapsed_time = CPUTime_Stop(clock);
                        }
                        /* In case time flag is not provided */
                        else {
                                (*map)(orig_img->pixels, (A2Methods_applyfun*) 
                                        rotate90, cl_trans);
                        }
                }
                if (angle == 270) {
                        /* Starts the timing of the rotation */
                        if (time) {
                                CPUTime_Start(clock);
                                (*map)(orig_img->pixels, (A2Methods_applyfun*) 
                                        rotate270, cl_trans);
                                elapsed_time = CPUTime_Stop(clock);
                        }       
                        /* In case time flag is not provided */
                        else {
                                (*map)(orig_img->pixels, (A2Methods_applyfun*) 
                                        rotate270, cl_trans);
                        }
                }
        }
        else if (angle == 180) {
                cl_maker(orig_img->width, orig_img->height, cl_trans, methods);
                /* Starts the timing of the rotation */
                if (time) {
                        CPUTime_Start(clock);
                }
                (*map)(orig_img->pixels, (A2Methods_applyfun*) 
                                rotate180, cl_trans);
                if (time) {
                        elapsed_time = CPUTime_Stop(clock);
                         }
        }
        
        /* Updates Pnm_ppm object with transformed values */
        orig_img->width = methods->width(cl_trans->raster);
        orig_img->height = methods->height(cl_trans->raster);
        methods->free(&(orig_img->pixels));
        orig_img->pixels = cl_trans->raster;

        /* Frees passed in allocated objects (no longer needed) */
        free(cl_trans);
        cl_trans = NULL;
        CPUTime_Free(&clock);
        
        /* Writes to stdout */
        Pnm_ppmwrite(stdout, orig_img);

        return elapsed_time;
}

void cl_maker(int width, int height, struct closure *cl, A2Methods_T methods) {
        A2Methods_UArray2 new_raster = methods->new(width, 
                height, sizeof(struct Pnm_rgb));
        cl->raster = new_raster;
        cl->arrayfxns = methods;
}

void rotate90(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans)
{
        struct closure *new_cl_rotation = cl_trans;

        int height = new_cl_rotation->arrayfxns->height(arr);

        struct Pnm_rgb pixel = (*(Pnm_rgb) elem);
        
        /* Reassigns pixel to new pos */
        (*(Pnm_rgb) (new_cl_rotation->arrayfxns->at(new_cl_rotation->raster, 
                     height - row - 1, col))) = pixel;
}

void rotate180(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans)
{
        struct closure *reflect = cl_trans;

        int height = reflect->arrayfxns->height(arr);
        
        int width = reflect->arrayfxns->width(arr);
        
        struct Pnm_rgb pixel = (*(Pnm_rgb) elem);
        
        /* Reassigns pixel to new pos */
        (*(Pnm_rgb) (reflect->arrayfxns->at(reflect->raster, width - col - 1, 
                     height - row - 1))) = pixel;
}

void rotate270(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans)
{
        struct closure *new_cl_rotation = cl_trans;

        int width = new_cl_rotation->arrayfxns->width(arr);

        struct Pnm_rgb pixel = (*(Pnm_rgb) elem);
        
        /* Reassigns pixel to new pos */
        (*(Pnm_rgb) (new_cl_rotation->arrayfxns->at(new_cl_rotation->raster, 
                     row, width - col - 1))) = pixel;
}

void rotate0(int col, int row, A2Methods_UArray2 arr, void *elem, 
        void *cl_trans)
{
        (void) arr;
        struct closure *new_cl_rotation = cl_trans;

        struct Pnm_rgb pixel = (*(Pnm_rgb) elem);
        
        /* Reassigns pixel to new pos */
        (*(Pnm_rgb) (new_cl_rotation->arrayfxns->at(new_cl_rotation->raster, 
                     col, row))) = pixel;
}