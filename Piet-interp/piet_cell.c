//
// Created by IWOFLEUR on 03.02.2026.
//
#include "piet_common.h"
#include "piet_cell.h"
#include "piet_color.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 *  Global grid variable definitions
 *
 *** see docs in piet_cell.h
 *** to identify it
 */
int width = 0;
int height = 0;
int *cells = NULL;

// Grid allocation and management block

/*
 * Allocate or resize the cell grid
 * new_width Desired grid width in codels (must be > 0)
 * new_height Desired grid height in codels (must be > 0)
 * Initializes new cells to black, preserves existing cells when resizing
 * Exits program with error message if allocation fails
 */
void piet_alloc_cells(int new_width, int new_height) {
    if (new_width <= 0 || new_height <= 0) {
        eprintf("error: invalid cell grid dimensions %dx%d\n", new_width, new_height);
        exit(EXIT_FAILURE);
    }

    int* new_cells = (int*)malloc(new_width * new_height * sizeof(int));
    if (new_cells == NULL) {
        eprintf("error: cannot allocate %dx%d cell grid (%ld bytes)\n", new_width, new_height,
                (long)(new_width * new_height * sizeof(int)));
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < new_height; j++) {
        for (int i = 0; i < new_width; i++) {
            new_cells[j * new_width + i] = C_BLACK;
        }
    }

    // If we have existiong cells, copy them to new grid
    if (cells != NULL) {
        int copy_width = MIN(width, new_width);
        int copy_height = MIN(height, new_height);

        for (int j = 0; j < copy_height; j++) {
            for (int i = 0; i < copy_width; i++) {
                new_cells[j * new_width + i] = cells[j * width + i];
            }
        }

        free(cells);
    }

    cells = new_cells;
    width = new_width;
    height = new_height;

    dprintf("debug: allocated %dx%d cell grid\n", width, height);
}

/*
 *  Helper function to determine codel size from image
 *  i Current position in scan (pixel index)
 *  c Color at curr position
 *  last_c Pointer to color at last color change
 *  last_p Pointer to position of last color change
 *  min_w Pointer to min run length found so far
 *  Used internally by piet_cleanup_input to guess codel size
 */
void piet_codel_size_check(int i, int c, int* last_c, int* last_p, int* min_w) {
    // Firstly we need to initialize tracking variables
    if (i == 0) {
        *last_c = c;
        *last_p = i;
    } else if (*last_c != c) {  // Color changed from previous pixel
        int run_length = i - *last_p;
        if (run_length < *min_w) {
            *min_w = run_length;
        }
        // Reset tracking for new color run
        *last_c = c;
        *last_p = i;
    }
    // If same color as previous there is nothing to do until color changed
}

/*
 *  Shrink input image by codel size (reduce resolution)
 *  If codel_size > 1, reduces grid dimensions by that factor
 *  by sampling one pixel per codel_size x code_size block
 *  Called after loading image to create codel-level representation
 */
void piet_cleanup_input(void) {
    int last_c, last_p;
    int min_width = width + 1;

    // Firstly we need to detect codel size if not specified horizontally
    if (codel_size < 0) {
        dprintf("debug: codel_size is negative\n");

        for (int row = 0; row < height; row++) {
            last_c = -1;
            last_p = 0;
            for (int col = 0; col < width; col++) {
                int cell_color = cells[row * width + col];
                piet_codel_size_check(col, cell_color, &last_c, &last_p, &min_width);
            }
            // End of row counts as an artificial color change
            piet_codel_size_check(width, C_MARK_INDEX, &last_c, &last_p, &min_width);
        }

        // Scan vert column by column for more accurate detection
        for (int col = 0; col < width; col++) {
            last_c = -1;
            last_p = 0;
            for (int row = 0; row < height; row++) {
                int cell_color = cells[row * width + col];
                piet_codel_size_check(row, cell_color, &last_c, &last_p, &min_width);
            }
            piet_codel_size_check(height, C_MARK_INDEX, &last_c, &last_p, &min_width);
        }

        // Use detected min run length as codel size
        codel_size = min_width;
        vprintf("info: codel_size is %d pixels\n", codel_size);
    }

    // Next - validate codel size
    if (codel_size <= 0) {
        eprintf("error: invalid codel size %d (m.b. positive)\n", codel_size);
        exit(EXIT_FAILURE);
    }

    if (width % codel_size != 0) {
        eprintf("error: codel size %d does not evenly divide image width %d\n", codel_size, width);
        exit(EXIT_FAILURE);
    }

    if (height % codel_size != 0) {
        eprintf("error: codel size %d does not evenly divide image height %d\n", codel_size, height);
        exit(EXIT_FAILURE);
    }

    // Perform resolution reduction as a last step here
    dprintf("debug: reducing resolution by factor %d\n", codel_size);

    int* original_cells = (int*)malloc(width * height * sizeof(int));
    if (original_cells == NULL) {
        eprintf("error: cannot allocate memory for cell grid backup\n");
        exit(EXIT_FAILURE);
    }
    memcpy(original_cells, cells, width * height * sizeof(int));

    int new_width = width / codel_size;
    int new_height = height / codel_size;
    piet_alloc_cells(new_width, new_height);

    /* Sample one pixel from each codel_size × codel_size block
    *  We sample the top-left pixel of each block
    *  Calculate position in original high-resolution image
    *  copy sampled pixels to new grid
    */
    for (int new_row = 0; new_row < new_height; new_row++) {
        for (int new_col = 0; new_col < new_width; new_col++) {
            int orig_row = new_row * codel_size;
            int orig_col = new_col * codel_size;
            int orig_index = orig_row * (new_width * codel_size) + orig_col;
            cells[new_row * new_width + new_col] = original_cells[orig_index];
        }
    }

    free(original_cells);
    dprintf("debug: resolution reduced from %dx%d to %dx%d codels\n", width * codel_size,
                height * codel_size, width, height);
}

/*
 *  Print entire cell grid to stdout for debugging
 *  Shows short names in a grid format
 *  Only compiled when DEBUG_ENABLED is defined
 */
void piet_dump_cells(void) {
#ifdef DEBUG_ENABLED
    if (!debug) return;

    printf("\n CELL GRID DUMP (%dx%d)\n", width, height);
    for (int row = 0; row < height; row++) {
        printf("Row %3d: ", row);
        for (int col = 0; col < width; col++) {
            printf("%3s", piet_cell_to_str(GET_CELL(col, row)));
        }
        printf("\n");
    }
    printf(" END GRID DUMP \n\n");
#endif
}