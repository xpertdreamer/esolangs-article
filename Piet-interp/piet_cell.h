//
// Created by IWOFLEUR on 03.02.2026.
//

#ifndef PIET_CELL_H
#define PIET_CELL_H

/*
 *  External declarations defined in piet_cell.c
 */

// Width and height of cell grid in codels (not pixels!)
extern int width, height;

// 1D array containing cell color indices (row-major)
extern int *cells;

/*
 * Cell access macroses and 'public' functions
 */

/*
 * Check if coordinates are within grid bounds
 * x X coordinate (0 = leftmost column)
 * y Y coordinate (0 = topmost row)
 * 1 if (x,y) is within grid, 0 otherwise
 */
#define CELL_IN_BOUNDS(x, y) ((x) >= 0 && (x) < width && (y) >= 0 && (y) < height)

/*
 * Convert 2D coordinates to 1D array index
 * x X coordinate
 * y Y coordinate
 * Array index for cells[] if coordinates are valid
 * -1 if coordinates are out of bounds
 * Uses row-major ordering: index = y * width + x
 */
#define CELL_IDX(x, y) (CELL_IN_BOUNDS(x, y) ? (y) * width + (x) : -1)

/*
 * Get color index at specified grid coordinates
 * x X coordinate
 * y Y coordinate
 * Color index at (x,y) if coordinates are valid
 * -1 if coordinates are out of bounds
 */
#define GET_CELL(x, y) (CELL_IN_BOUNDS(x, y) ? cells[CELL_IDX(x, y)] : -1)

/*
 * Set color index at specified grid coordinates
 * x X coordinate
 * y Y coordinate
 * val New color index to store
 * Does nothing if coordinates are out of bounds
 * Use with caution - may corrupt program state if used incorrectly
 */
#define SET_CELL(x, y, val) do { \
                              int idx = CELL_IDX(x, y); \
                              if (idx >= 0) cells[idx] = (val); \
                            } while (0)

/*
 * Allocate or resize the cell grid
 * new_width Desired grid width in codels (must be > 0)
 * new_height Desired grid height in codels (must be > 0)
 * Initializes new cells to black, preserves existing cells when resizing
 * Exits program with error message if allocation fails
 */
void piet_alloc_cells(int new_width, int new_height);

/*
 *  Shrink input image by codel size (reduce resolution)
 *  If codel_size > 1, reduces grid dimensions by that factor
 *  by sampling one pixel per codel_size x code_size block
 *  Called after loading image to create codel-level representation
 */
void piet_cleanup_input(void);

/*
 *  Print entire cell grid to stdout for debugging
 *  Shows short names in a grid format
 *  Only compiled when DEBUG_ENABLED is defined
 */
void piet_dump_cells(void);

/*
 *  Helper function to determine codel size from image
 *  i Current position in scan (pixel index)
 *  c Color at curr position
 *  last_c Pointer to color at last color change
 *  last_p Pointer to position of last color change
 *  min_w Pointer to min run length found so far
 *  Used internally by piet_cleanup_input to guess codel size
 */
void piet_codel_size_check(int i, int c, int* last_c, int* last_p, int* min_w);

#endif //PIET_CELL_H