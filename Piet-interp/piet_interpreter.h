//
// Created by IWOFLEUR on 03.02.2026.
//

#ifndef PIET_INTERPRETER_H
#define PIET_INTERPRETER_H

#define MAX_STEPS 1000

/*
 * Direction constants for dp (direction pointer)
 * 4 directions according to Piet specification
 * Below we have direction manipulation macroses
 */
#define PIET_LEFT 'l'
#define PIET_RIGHT 'r'
#define PIET_UP 'u'
#define PIET_DOWN 'd'

/*
 * Toggle codel chooser (cc) between left and right
 * cc Current codel chooser value (PIET_LEFT or PIET_RIGHT)
 * Return opposite codel chooser value
 */
#define TOGGLE_CC(cc) ((cc) == 'r' ? 'l' : 'r')

/*
 * Rotate direction pointer (dp) clockwise by 90 degrees
 * dp Current direction pointer value
 * Return new direction after clockwise rotation
 * NOTE: Rotation order: right → down → left → up → right
 */
#define TURN_DP(dp) ((dp) == 'r' ? 'd' : ((dp) == 'd' ? 'l' : \
                        ((dp) == 'l' ? 'u' : 'r')))

/*
 * Rotate direction pointer (DP) counter-clockwise by 90 degrees
 * dp Current direction pointer value
 * Return new direction after counter-clockwise rotation
 * NOTE: Rotation order: right → up → left → down → right
 */
#define TURN_DP_INV(dp)     ((dp) == 'r' ? 'u' : ((dp) == 'u' ? 'l' : \
((dp) == 'l' ? 'd' : 'r')))

/*
 * Get X-axis movement delta for given direction
 * dp Direction pointer value
 * Return -1 for left, +1 for right, 0 for up/down
 */
#define DP_DX(dp) ((dp) == 'l' ? -1 : ((dp) == 'r' ? 1 : 0))

/*
 * Get Y-axis movement delta for given direction
 * dp Direction pointer value
 * Return -1 for up, +1 for down, 0 for left/right
 */
#define DP_DY(dp) ((dp) == 'u' ? -1 : ((dp) == 'd' ? 1 : 0))

/*
 *  Execution state variables
 *  (defined in piet_interpreter.c)
 *  p_dir_pointer Current direction pointer (DP) value
 *  p_codel_chooser Current codel chooser (CC) value
 *  p_xpos Current X position in cell grid
 *  p_ypos Current Y position in cell grid
 */
extern int p_dir_pointer;
extern int p_codel_chooser;
extern int p_xpos;
extern int p_ypos;

// 'Public' funtions prototypes

/*
 * Initialize interpreter to starting state
 * Sets DP to right, CC to left, position to (0,0), clears step counter
 * Should be called before running any Piet program
 */
void piet_init(void);

/*
 * Run Piet program to completion
 * Return 0 on normal termination, -1 on error
 * Main execution loop that repeatedly calls piet_step()
 * until program terminates or error occurs
 */
int piet_run(void);

/*
 * Execute single step of Piet program
 * Return 0 on successful step, -1 on termination/error
 * Finds next codel, executes command if any, updates state
 * Handles white sliding, black blocking, and all Piet commands
 */
int piet_step(void);

/*
 * Execute Piet command based on color transition
 * c_col Color index of current block
 * a_col Color index of adjacent block (where we're moving to)
 * num_cells Size of current color block (for PUSH command)
 * msg Buffer to store command description (min 16 chars)
 * Return 0 on success, -1 on error (currently no errors defined)
 * Implements Piet's 18 commands based on hue/lightness changes
 */
int piet_action(int c_col, int a_col, int num_cells, char *msg);

/*
 * Find edge codel of current color block in DP/CC direction
 * n_x Pointer to store X coordinate of edge codel
 * n_y Pointer to store Y coordinate of edge codel
 * num_cells Pointer to store size of color block
 * Return 0 on success, -1 on error (invalid position)
 * Uses flood fill to explore color block and find furthest
 * codel in current DP direction, breaking ties with CC
 */
int piet_walk_border(int *n_x, int *n_y, int *num_cells);

/*
 * Slide through white cells in current direction
 * n_x Pointer to starting X (updated to final position)
 * n_y Pointer to starting Y (updated to final position)
 * Return 0 on success, -1 on error (should not occur)
 * Implements Piet's white block sliding - continues moving
 * in DP direction while cells are white
 */
int piet_walk_white(int *n_x, int *n_y);

/*
 * Flood fill helper to find connected cells
 * x Current X coordinate in recursion
 * y Current Y coordinate in recursion
 * c_idx Target color index to match
 * c_mark Temporary marker color for visited cells
 * n_x Pointer to best X found so far (updated if better found)
 * n_y Pointer to best Y found so far (updated if better found)
 * num_cells Pointer to cell counter (incremented for each cell)
 * Return 0 if cell was processed, -1 if cell doesn't match or is invalid
 * Recursive depth-first search marking visited cells with c_mark
 */
int piet_check_connected_cell(int x, int y, int c_idx, int c_mark,
                                int* n_x, int* n_y, int* num_cells);

/*
 * Reset flood fill markings to original colors
 * x Current X coordinate in recursion
 * y Current Y coordinate in recursion
 * c_idx Original color index to restore
 * c_mark Marker color to find and replace
 * Return 0 if cell was reset, -1 if cell doesn't have marker
 * Recursively finds cells marked with c_mark and restores c_idx
 */
int piet_reset_connected_cell(int x, int y, int c_idx, int c_mark);

#endif //PIET_INTERPRETER_H