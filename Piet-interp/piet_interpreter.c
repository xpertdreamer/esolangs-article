//
// Created by IWOFLEUR on 03.02.2026.
//

#include "piet_common.h"
#include "piet_interpreter.h"

#include <limits.h>

#include "piet_color.h"
#include "piet_cell.h"
#include "piet_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  Execution state variable definitions
 *  (see piet_interpreter.h for specification)
 */
int p_dir_pointer = PIET_RIGHT;
int p_codel_chooser = PIET_LEFT;
int p_xpos = 0;
int p_ypos = 0;


/*
 * Initialize interpreter to starting state
 * Sets DP to right, CC to left, position to (0,0), clears step counter
 * Should be called before running any Piet program
 */
void piet_init(void) {
    p_dir_pointer = PIET_RIGHT;
    p_codel_chooser = PIET_LEFT;
    p_xpos = p_ypos = 0;
    exec_step = 0;
    num_stack = 0;
    dprintf("debug: interpreter initialized: dp=%c, cc=%c at (%d,%d)\n",
            p_dir_pointer, p_codel_chooser, p_xpos, p_ypos);
}

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
                                int* n_x, int* n_y, int* num_cells) {
    int current_color = GET_CELL(x, y);
    int found_better = 0;

    // Check if position is valid (within grid)
    if (current_color < 0) return -1;

    // Check if cell has target color and isn't already marked
    if (current_color != c_idx || current_color == c_mark) return -1;

    // Evaluate if this codel is better than current best according to dp/cc
    if (p_dir_pointer == PIET_LEFT && x <= *n_x) {
        // Looking left: prefer smaller X (more left), break ties with CC
        if (x < *n_x ||  // Definitely better if more left
            (p_codel_chooser == PIET_LEFT && y > *n_y) ||  // Tie: CC=left prefers larger Y
            (p_codel_chooser == PIET_RIGHT && y < *n_y)) { // Tie: CC=right prefers smaller Y
            found_better = 1;
            }
    } else if (p_dir_pointer == PIET_RIGHT && x >= *n_x) {
        // Looking right: prefer larger X (more right), break ties with CC
        if (x > *n_x ||  // Definitely better if more right
            (p_codel_chooser == PIET_LEFT && y < *n_y) ||  // Tie: CC=left prefers smaller Y
            (p_codel_chooser == PIET_RIGHT && y > *n_y)) { // Tie: CC=right prefers larger Y
            found_better = 1;
            }
    } else if (p_dir_pointer == PIET_UP && y <= *n_y) {
        // Looking up: prefer smaller Y (more up), break ties with CC
        if (y < *n_y ||  // Definitely better if more up
            (p_codel_chooser == PIET_LEFT && x < *n_x) ||  // Tie: CC=left prefers smaller X
            (p_codel_chooser == PIET_RIGHT && x > *n_x)) { // Tie: CC=right prefers larger X
            found_better = 1;
            }
    } else if (p_dir_pointer == PIET_DOWN && y >= *n_y) {
        // Looking down: prefer larger Y (more down), break ties with CC
        if (y > *n_y ||  // Definitely better if more down
            (p_codel_chooser == PIET_LEFT && x > *n_x) ||  // Tie: CC=left prefers larger X
            (p_codel_chooser == PIET_RIGHT && x < *n_x)) { // Tie: CC=right prefers smaller X
            found_better = 1;
            }
    }

    if (found_better) {
        *n_x = x;
        *n_y = y;
        d2printf("debug: new best codel at (%d,%d) for dp=%c, cc=%c", x, y,
                p_dir_pointer, p_codel_chooser);
    }

    /*
     * Mark cell as visited (temporarily change its color)
     * and count cell as a part of a black color
     */
    SET_CELL(x, y, c_mark);
    (*num_cells)++;

    /*
     * Recursively explore neighbours in order:
     * right (x+1, y)
     * left (x-1, y)
     * down (x, y+1)
     * up (x, y-1)
     */
    piet_check_connected_cell(x + 1, y, c_idx, c_mark, n_x, n_y, num_cells);
    piet_check_connected_cell(x - 1, y, c_idx, c_mark, n_x, n_y, num_cells);
    piet_check_connected_cell(x, y + 1, c_idx, c_mark, n_x, n_y, num_cells);
    piet_check_connected_cell(x, y - 1, c_idx, c_mark, n_x, n_y, num_cells);

    return 0;
}

/*
 * Reset flood fill markings to original colors
 * x Current X coordinate in recursion
 * y Current Y coordinate in recursion
 * c_idx Original color index to restore
 * c_mark Marker color to find and replace
 * Return 0 if cell was reset, -1 if cell doesn't have marker
 * Recursively finds cells marked with c_mark and restores c_idx
 */
int piet_reset_connected_cell(int x, int y, int c_idx, int c_mark) {
    int current_color = GET_CELL(x, y);

    // Check if this cell has the marker (and is within bounds)
    // Then restore original color
    if (current_color < 0 || current_color != c_mark) return -1;
    SET_CELL(x, y, c_idx);

    /*
     * Recursively reset all neighboring marked cells
     * Order of restoring is identity to exploring in
     * piet_check_connected_cell function
     */
    piet_reset_connected_cell(x + 1, y, c_idx, c_mark);
    piet_reset_connected_cell(x - 1, y, c_idx, c_mark);
    piet_reset_connected_cell(x, y + 1, c_idx, c_mark);
    piet_reset_connected_cell(x, y - 1, c_idx, c_mark);

    return 0;
}

/*
 * Find edge codel of current color block in DP/CC direction
 * n_x Pointer to store X coordinate of edge codel
 * n_y Pointer to store Y coordinate of edge codel
 * num_cells Pointer to store size of color block
 * Return 0 on success, -1 on error (invalid position)
 * Uses flood fill to explore color block and find furthest
 * codel in current DP direction, breaking ties with CC
 */
int piet_walk_border(int *n_x, int *n_y, int *num_cells) {
    int current_color = GET_CELL(p_xpos, p_ypos);

    /*
     * Validate starting position
     * In case of successfull validation - initialize
     * search starting point and counters
     * and perform flood fill to explore entire color block
     */
    if (current_color < 0) {
        eprintf("error: invalid starting position (%d,%d)\n", p_xpos, p_ypos);
        return -1;
    }

    *n_x = p_xpos;
    *n_y = p_ypos;
    *num_cells = 0;

    int result = piet_check_connected_cell(p_xpos, p_ypos, current_color,
                                            C_MARK_INDEX, n_x, n_y, num_cells);
    if (result >= 0)
        piet_reset_connected_cell(p_xpos, p_ypos, current_color, C_MARK_INDEX);

    d2printf("debug: border walk result: edge at (%d,%d) block size=%d\n", *n_x, *n_y, *num_cells);
    return result;
}

/*
 * Slide through white cells in current direction
 * n_x Pointer to starting X (updated to final position)
 * n_y Pointer to starting Y (updated to final position)
 * Return 0 on success, -1 on error (should not occur)
 * Implements Piet's white block sliding - continues moving
 * in DP direction while cells are white
 */
int piet_walk_white(int *n_x, int *n_y) {
    int current_color = GET_CELL(*n_x, *n_y);
    d2printf("debug: starting white walk from (%d,%d)\n", *n_x, *n_y);

    // Continue moving while current cell is white
    while (piet_is_white(current_color)) {
        *n_x += DP_DX(p_dir_pointer);
        *n_y += DP_DY(p_dir_pointer);

        current_color = GET_CELL(*n_x, *n_y);
        d2printf("debug: white slide to (%d,%d) color=%s\n", *n_x, *n_y, piet_cell_to_str(current_color));

        // Safety check: avoid infinite loop if something goes wrong
        if (!CELL_IN_BOUNDS(*n_x, *n_y)) break;
    }

    d2printf("debug: white walk ended at (%d,%d) color %s\n", *n_x, *n_y, piet_cell_to_str(current_color));
    return 0;
}

/*
 * Execute Piet command based on color transition
 * c_col Color index of current block
 * a_col Color index of adjacent block (where we're moving to)
 * num_cells Size of current color block (for PUSH command)
 * msg Buffer to store command description (min 16 chars)
 * Return 0 on success, -1 on error (currently no errors defined)
 * Implements Piet's 18 commands based on hue/lightness changes
 */
int piet_action(int c_col, int a_col, int num_cells, char *msg) {
    /*
     * Calculate hue and lightness differences between colors
     * Add N_HUE/N_LIGHT before modulo to ensure positive results
     */
    int hue_change = ((piet_get_hue(a_col) - piet_get_hue(c_col)) + N_HUE) % N_HUE;
    int light_change = ((piet_get_light(a_col) - piet_get_light(c_col)) + N_LIGHT) % N_LIGHT;

    // Default message (should be overwritten by command execution)
    strcpy(msg, "unknown");
    t2printf("action: transition %s -> %s: hue delta=%d, lightness delta=%d\n",
             piet_cell_to_str(c_col), piet_cell_to_str(a_col),
             hue_change, light_change);

    switch (hue_change) {
        /*
         * No hue change means stack manipulation commands
         * Light change table should look like this according to Piet specs:
         *
         * light_change == 1 PUSH: Push block size onto stack
         * light_change == 2 POP: Pop top value from stack
         */
        case 0: {
            if (light_change == 1) {
                strcpy(msg, "push");
                STACK_PUSH(num_cells);
                tprintf("action: PUSH %d\n", num_cells);
            } else if (light_change == 2) {
                strcpy(msg, "pop");
                if (num_stack > 0) {
                    num_stack--;
                    tprintf("action: POP\n");
                } else tprintf("action: POP failed - stack undeflow\n");
            }
            break;
        }

        /*
         *  Hue change 1 means arithmetic commands
         *  Light change table should look like this according to Piet specs:
         *
         *  light_change == 0 ADD: Pop two values, push their sum
         *  light_change == 1 SUB: Pop two values, push second minus first
         *  light_change == 2 MUL: Pop two values, push their product
         */
        case 1: {
            if (light_change == 0) {
                strcpy(msg, "add");
                if (num_stack >= 2) {
                    stack[num_stack - 2] += stack[num_stack - 1];
                    num_stack--;
                    tprintf("action: ADD %ld + %ld = %ld\n",
                            stack[num_stack - 1], stack[num_stack],
                            stack[num_stack - 1]);
                } else tprintf("action: ADD failed - stack underflow\n");
            } else if (light_change == 1) {
                strcpy(msg, "sub");
                if (num_stack >= 2) {
                    stack[num_stack - 2] -= stack[num_stack - 1];
                    num_stack--;
                    tprintf("action: SUB %ld - %ld = %ld\n",
                            stack[num_stack - 1], stack[num_stack],
                            stack[num_stack - 1]);
                } else tprintf("action: SUB failed - stack underflow\n");
            } else if (light_change == 2) {
                strcpy(msg, "mul");
                if (num_stack >= 2) {
                    stack[num_stack - 2] *= stack[num_stack - 1];
                    num_stack--;
                    tprintf("action: MUL %ld * %ld = %ld\n",
                            stack[num_stack - 1], stack[num_stack],
                            stack[num_stack - 1]);
                } else tprintf("action: MUL failed - stack underflow\n");
            }
            break;
        }

         /*
         *  Hue change 2 means division, modulo and logical commands
         *  Light change table should look like this according to Piet specs:
         *
         *  light_change == 0 DIV: Integer division (second/first).
         *                         Division by zero: push large error value (Piet spec)
         *  light_change == 1 MOD: Remainder of integer division,
         *                         Modulo by zero: result is undefined, leave unchanged
         *  light_change == 2 NOT: Logical negation (0->1, non-zero->0)
         */
        case 2: {
            if (light_change == 0) {
                strcpy(msg, "div");
                if (num_stack >= 2) {
                    long divisor = stack[num_stack -1];
                    if (divisor != 0) {
                        stack[num_stack - 2] /= divisor;
                        tprintf("action: DIV %ld / %ld = %ld\n",
                                stack[num_stack - 2], divisor,
                                stack[num_stack - 2]);
                    } else {
                        stack[num_stack - 2] = LONG_MAX;
                        tprintf("action: division by zero (pushed LONG_MAX)\n");
                    }
                    num_stack--;
                } else tprintf("action: DIV failed - stack underflow\n");
            } else if (light_change == 1) {
                strcpy(msg, "mod");
                if (num_stack >= 2) {
                    long divisor = stack[num_stack - 1];
                    if (divisor != 0) {
                        stack[num_stack - 2] %= divisor;
                        tprintf("action: MOD %ld %% %ld = %ld\n",
                                stack[num_stack - 2], divisor,
                                stack[num_stack - 2]);
                    } else tprintf("action: MOD by zero (no change)\n");
                    num_stack--;
                } else tprintf("action: MOD failed - stack underflow\n");
            } else if (light_change == 2) {
                strcpy(msg, "not");
                if (num_stack >= 1) {
                    stack[num_stack - 1] = !stack[num_stack - 1];
                    tprintf("action: NOT %ld->%ld\n", !stack[num_stack - 1], stack[num_stack - 1]);
                } else tprintf("action: NOT failed - stack underflow\n");
            }
            break;
        }

        /*
         *  Hue change 3 means comparison and conttol flow commands
         *  Light change table should look like this according to Piet specs:
         *
         *  light_change == 0 GREATER: Compare two values (second > first)
         *                     a b -> (a > b) ? 1 : 0
         *  light_change == 1 POINTER: Rotate dp based on stack value
         *                    Positive: rotate dp clockwise, negative: counter-clockwise
         *  light_change == 2 SWITCH: Toggle cc based on stack value
         */
        case 3: {
            if (light_change == 0) {
                strcpy(msg, "gt");
                if (num_stack >= 2) {
                    int res = stack[num_stack - 2] > stack[num_stack - 1] ? 1 : 0;
                    stack[num_stack - 2] = res;
                    num_stack--;
                    tprintf("action: GREATER %ld > %ld = %d\n",
                            stack[num_stack - 1], stack[num_stack], res);
                } else tprintf("action: GREATER failed - stack underflow\n");
            } else if (light_change == 1) {
                strcpy(msg, "dp");
                if (num_stack >= 1) {
                    int rotations = STACK_POP();
                    if (rotations > 0) {
                        for (int i = 0; i < rotations; i++) {
                            p_dir_pointer = TURN_DP(p_dir_pointer);
                        }
                        tprintf("action: POINTER rotate dp %d steps clockwise → %c\n",
                                rotations, p_dir_pointer);
                    } else if (rotations < 0) {
                        for (int i = 0; i > rotations; i--)
                            p_dir_pointer = TURN_DP_INV(p_dir_pointer);
                        tprintf("action: POINTER rotate dp %d steps counter-clockwise → %c\n",
                                -rotations, p_dir_pointer);
                    } else tprintf("action: pointer 0 rotations");
                } else tprintf("action: pointer failed - stack underflow");
            } else if (light_change == 2) {
                strcpy(msg, "cc");
                if (num_stack >= 1) {
                    int toggles = STACK_POP();
                    for (int i = 0; i < toggles; i++) {
                        p_codel_chooser = TOGGLE_CC(p_codel_chooser);
                    }
                    tprintf("action: SWITCH toggle cc %d times -> %c\n", toggles, p_codel_chooser);
                }
            }
            break;
        }

       /*
        *  Hue change 4 means stack manipulation and input commands
        *  Light change table should look like this according to Piet specs:
        *
        *  light_change == 0 DUP: Copy top value on stack
        *  light_change == 1 ROLL: Roll stack values (complex operation)
        *                          Pops number of rolls and how deep to roll from stack
        *                          Shift array right by 'rolls' positions
        *  light_change == 2 IN (NUMBER): Read integer from stdin
        *                                 Shows prompt for input (unless quiet mode)
        */
        case 4: {
            if (light_change == 0) {
                strcpy(msg, "dup");
                if (num_stack >= 1) {
                    STACK_PUSH(stack[num_stack - 1]);
                    tprintf("action: DUP %ld\n", stack[num_stack - 1]);
                } else tprintf("action: DUP failed - stack underflow\n");
            } else if (light_change == 1) {
                strcpy(msg, "roll");
                if (num_stack >= 2) {
                    int rolls = STACK_POP();
                    int depth = STACK_POP();
                    tprintf("action: ROLL depth=%d rolls=%d\n", depth, rolls);

                    if (depth > 0 && num_stack >= depth) {
                        rolls = rolls % depth;
                        if (rolls < 0) rolls += depth;

                        if (rolls > 0) {
                            long *temp = (long *)malloc(sizeof(long) * depth);
                            if (temp != NULL) {
                                for (int i = 0; i < depth; i++) {
                                    temp[i] = stack[num_stack - depth + i];
                                }

                                for (int i = 0; i < depth; i++) {
                                    int new_pos = (i + rolls) % depth;
                                    stack[num_stack - depth + i] = temp[new_pos];
                                }

                                free(temp);
                                tprintf("action: ROLL completed successfully\n");
                            } else tprintf("action: ROLL failed - memory error\n");
                        } else tprintf("action: ROLL 0 rotations\n");
                    } else tprintf("action: ROLL failed - invalid depth or stack underflow\n");
                } else tprintf("action: ROLL failed - stack underflow\n");
            } else if (light_change == 2) {
                strcpy(msg, "inN");
                if (!quiet) {
                    printf("? ");
                    fflush(stdout);
                }

                long input_val;
                if (scanf("%ld", &input_val) == 1) {
                    STACK_PUSH(input_val);
                    tprintf("action: IN(number) read %ld\n", input_val);
                } else tprintf("action: IN(number) failed to read input\n");
            }
            break;
        }

        /*
         *  Hue change 5 means input/output commands
         *  Light change table should look like this according to Piet specs:
         *
         *  light_change == 0 IN(CHAR): Read character from stdin
         *                    Shows prompt for input (unless quiet mode)
         *                    Value pushed as 8-bit
         *  light_change == 1 OUT(NUMBER): Print number to stdout
         *  light_change == 2 OUT(CHAR): Print character to stdout
         *                    Value printed as ASCII character
         */
        case 5: {
            if (light_change == 0) {
                strcpy(msg, "inC");
                if (!quiet) {
                    printf("? ");
                    fflush(stdout);
                }

                int input_ch = getchar();
                if (input_ch != EOF) {
                    STACK_PUSH(input_ch & 0xFF);
                    tprintf("action: IN(char) read '%c' (ASCII %d)\n",
                            (input_ch >= 32 && input_ch < 127) ? input_ch : '.',
                            input_ch);
                } else tprintf("action: IN(char) failed - EOF\n");
            } else if (light_change == 1) {
                strcpy(msg, "outN");
                if (num_stack >= 1) {
                    long val = STACK_POP();
                    printf("%ld", val);
                    fflush(stdout);
                    tprintf("action: OUT(number) written %ld\n", val);
                } else tprintf("action: OUT(number) failed - stack underflow\n");
            } else if (light_change == 2) {
                strcpy(msg, "outC");
                if (num_stack >= 1) {
                    long val = STACK_POP();
                    printf("%c", (char)(val & 0xFF));
                    fflush(stdout);
                    tprintf("action: OUT(char) printed '%c' (ASCII %ld)\n",
                            (val >= 32 && val < 127) ? (char)val : '.', val);
                } else tprintf("action: OUT(char) failed - stack underflow\n");
            }
            break;
        }

        /*
         * In default case we have unknown hue change
         * (should not happen with valid Piet colors)
         * In this case just throw error and break cycle - that would be enough
         */
        default: {
            eprintf("error: invalid hue change %d in piet_action()\n", hue_change);
            break;
        }
    }

    piet_dump_stack();
    return 0;
}

/*
 * Execute single step of Piet program
 * Return 0 on successful step, -1 on termination/error
 * Finds next codel, executes command if any, updates state
 * Handles white sliding, black blocking, and all Piet commands
 */
int piet_step(void) {
    /*
     * Here some variables we need for step. Explanation:
     * tries Attempt counter for finding valid move
     * edge_x, edge_y Edge codel of current block
     * target_x, target_y Cell we're trying to move to
     * pre_x, pre_y, pre_dp, pre_cc Save state for tracing
     * current_color, target_color Colors at current and target positions
     * block_size  Size of current color block (for PUSH)
     * white_crossed Flag: did we slide through white?
     * in_white_block Flag: are we currently on white?
     * static toggle_counter Flag: are we currently on white?
     * action_msg[] Buffer for command description
     */
    int tries;
    int edge_x, edge_y;
    int target_x, target_y;
    int pre_x, pre_y, pre_dp, pre_cc;
    int current_color, target_color;
    int block_size;
    int white_crossed = 0;
    int in_white_block = 0;
    static int toggle_counter = 0;
    char action_msg[32];

    /*
     * First - do some pre-execution steps:
     *         Check execution step limit
     *         Get color at current position
     *         Handle special case: starting on black cell (program error)
     *         Save current state for tracing and potential rollback
     *         Determine if we're on white (affects movement rules)
     */
    if (max_exec_step > 0 && exec_step >= max_exec_step) {
        eprintf("error: max_exec_step (%u) exceeded\n", max_exec_step);
        return -1;
    }

    current_color = GET_CELL(p_xpos, p_ypos);
    if (piet_is_black(current_color)) {
        tprintf("trace: starting on black cell - program terminated\n");
        return -1;
    }

    pre_x = p_xpos;
    pre_y = p_ypos;
    pre_dp = p_dir_pointer;
    pre_cc = p_codel_chooser;
    white_crossed = piet_is_white(current_color);

    // Attempt to find valid move (up to 8 tries per Piet spec)
    for (tries = 0; tries < 8; tries++) {
        edge_x = p_xpos;
        edge_y = p_ypos;

        if (piet_is_white(current_color)) {
            block_size = 1;
            d2printf("debug: in white cell at (%d,%d)\n", p_xpos, p_ypos);
        } else {
            if (piet_walk_border(&edge_x, &edge_y, &block_size) < 0) {
                eprintf("error: failed to walk border from (%d,%d)\n", p_xpos, p_ypos);
                return -1;
            }
            d2printf("debug: edge codel at (%d,%d), block size=%d\n", edge_x, edge_y, block_size);
        }

        // Calculate targeet position (one step in dp direction)
        target_x = edge_x + DP_DX(p_dir_pointer);
        target_y = edge_y + DP_DY(p_dir_pointer);
        target_color = GET_CELL(target_x, target_y);
        d2printf("debug: try %d: target (%d,%d) color=%s\n",
                 tries, target_x, target_y, piet_cell_to_str(target_color));

        // Handle white block sliding
        if (piet_is_white(target_color)) {
            d2printf("debug: target is white, starting slide\n");
            int slide_x = target_x;
            int slide_y = target_y;

            while (piet_is_white(GET_CELL(slide_x, slide_y))) {
                slide_x += DP_DX(p_dir_pointer);
                slide_y += DP_DY(p_dir_pointer);
            }

            target_x = slide_x;
            target_y = slide_y;
            target_color = GET_CELL(target_x, target_y);
            d2printf("debug: after slide: target (%d,%d) color=%s\n",
                     target_x, target_y, piet_cell_to_str(target_color));

            if (target_color >= 0 && !piet_is_black(target_color)) white_crossed = 1;
            else {
                d2printf("debug: hit black/edge during white slide\n");
                target_x -= DP_DX(p_dir_pointer);
                target_y -= DP_DY(p_dir_pointer);
                target_color = C_WHITE;
                white_crossed = 1;
                d2printf("debug: Backed up to (%d,%d) as white\n", target_x, target_y);
            }
        }

        // Check if target is valid
        if (target_color < 0 || piet_is_black(target_color)) {
            d2printf("debug: invalid target - toggling direction\n");
            if (piet_is_white(current_color) || in_white_block) {
                p_codel_chooser = TOGGLE_CC(p_codel_chooser);
                p_dir_pointer = TURN_DP(p_dir_pointer);
                d2printf("debug: in white - toggle cc to %c, dp to %c\n",
                         p_codel_chooser, p_dir_pointer);
            } else {
                if (toggle_counter % 2 == 0) {
                    p_codel_chooser = TOGGLE_CC(p_codel_chooser);
                    d2printf("debug: toggle cc to %c (try %d)\n",
                             p_codel_chooser, tries);
                } else {
                    p_dir_pointer = TURN_DP(p_dir_pointer);
                    d2printf("debug: toggle dp to %c (try %d)\n",
                             p_dir_pointer, tries);
                }
                toggle_counter++;
            }

            continue;
        }

        // If valid move found - execute step
        tprintf("\ntrace: step %u  (%d,%d/%c,%c %s -> %d,%d/%c,%c %s):\n",
                exec_step, pre_x, pre_y, pre_dp, pre_cc,
                piet_cell_to_str(current_color),
                target_x, target_y, p_dir_pointer, p_codel_chooser,
                piet_cell_to_str(target_color));

        exec_step++;
        if (white_crossed) {
            strcpy(action_msg, "noop");
            t2printf("action: no command (white block slide)\n");
        } else piet_action(current_color, target_color, block_size, action_msg);

        p_xpos = target_x;
        p_ypos = target_y;

        t2printf("trace: step complete - moved to (%d,%d)\n", p_xpos, p_ypos);
        return 0;
    }

    tprintf("trace: exhausted all 8 movement attempts - program terminated\n");
    return -1;
}

/*
 * Run Piet program to completion
 * Return 0 on normal termination, -1 on error
 * Main execution loop that repeatedly calls piet_step()
 * until program terminates or error occurs
 */
int piet_run(void) {
    if (width <= 0 || height <= 0) {
        eprintf("error: no program loaded -> empty cell grid\n");
        return -1;
    }

    piet_init();

    while (1) {
        t2printf("trace: state: pos=(%d,%d) DP=%c CC=%c\n",
               p_xpos, p_ypos, p_dir_pointer, p_codel_chooser);

        if (exec_step > MAX_STEPS) {
            eprintf("\nerror: possible infinite loop detected after %lu steps\n", exec_step);
            eprintf("       (last position: %d,%d DP=%c CC=%c)\n",
                    p_xpos, p_ypos, p_dir_pointer, p_codel_chooser);
            return 0;
        }

        if (piet_step() < 0) {
            vprintf("\ninfo: program terminated after %u steps\n", exec_step);
            break;
        }
    }

    return 0;
}