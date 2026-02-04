//
// Created by IWOFLEUR on 03.02.2026.
//

#include "piet_stack.h"
#include "piet_common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Stack variable definitions
 * (see piet_stack.h for specification)
 */
long* stack = NULL;
int num_stack = 0;
int max_stack = 0;

/*
 * Stack memory management
 * and debugging utilities
 */

/*
 *  Ensure stack has minimum capacity
 *  min_size Minimum number of elements stack must hold
 *  Expands stack allocation if current capacity < min_size
 *  Uses geometric growth to avoid frequent reallocations
 */
void piet_alloc_stack_space(int min_size) {
    // If stack already has enough space - nothing to do
    if (min_size <= max_stack) {
        return;
    }

    if (stack == NULL) {
        max_stack = min_size;
        SAFE_ALLOC(stack, long, min_size);
        dprintf("debug: initial stack allocation: %d entries\n", max_stack);
    } else {
        int new_size = max_stack * 2;
        if (new_size < min_size) new_size = min_size;

        max_stack = new_size;
        SAFE_REALLOC(stack, long, max_stack);
        dprintf("debug: stack expanded to %d entries (currently using %d)\n",
                max_stack, num_stack);
    }
}

/*
 *  Print stack contents for debugging/tracing
 *  Shows all values from bottom to top when trace is enabled
 *  Only outputs when trace is enabled and tprintf() is active
 */
void piet_dump_stack(void) {
#ifdef TRACE_ENABLED
    if (!trace) return;

    if (num_stack == 0) {
        tprintf("trace: stack is empty\n");
        return;
    }

    tprintf("trace: stack (%d values, bottom->top):", num_stack);
    for (int i = 0; i < num_stack; i++)
        tprintf(" %ld", stack[i]);
    tprintf("\n");
#endif
}

/*
 *  Free all stack memory and reset stack state
 *  Called at program termination to clean up resources
 */
void piet_stack_cleanup(void) {
    if (stack != NULL) {
        free(stack);
        stack = NULL;
        num_stack = 0;
        max_stack = 0;
        dprintf("debug: stack cleaned up\n");
    }
}