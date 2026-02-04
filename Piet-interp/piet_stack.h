//
// Created by IWOFLEUR on 03.02.2026.
//

#ifndef PIET_STACK_H
#define PIET_STACK_H

/*
 *  External stack variable declarations
 *  stack Dynamic array containing stack data (grows upward)
 *  num_stack Current number of values stored on the stack
 *  max_stack Current allocated size of stack array
 */
extern long *stack;
extern int num_stack;
extern int max_stack;

/*
 * Stack operation macroses
 * (safe wrappers)
 * and some prototypes down there
 */

/*
 * Push a value onto the stack with automatic expansion
 * val Value to push (long integer)
 * Automatically allocates more memory if stack is full
 * Uses piet_alloc_stack_space() internally
 */
#define STACK_PUSH(val) do { \
                            piet_alloc_stack_space(num_stack + 1); \
                            stack[num_stack++] = (val); \
                        } while (0)

/*
 *  Pop a value from the stack
 *  Return top value from stack, or 0 if stack is empty
 */
#define STACK_POP() (num_stack > 0 ? stack[--num_stack] : 0)

/*
 *  Get top value without removing it
 *  Top value from stack, or 0 if stack is empty
 *  Does not modify stack - just peeks at top value
 */
#define STACK_PEEK() (num_stack > 0 ? stack[num_stack - 1] : 0)

/*
 *  Get second value from top without removing it
 *  Second value from stack top, or 0 if stack has < 2 values
 *  Useful for binary operations (add, subtract, etc.)
 */
#define STACK_SECOND() (num_stack > 1 ? stack[num_stack - 2] : 0)

/*
 *  Ensure stack has minimum capacity
 *  min_size Minimum number of elements stack must hold
 *  Expands stack allocation if current capacity < min_size
 *  Uses geometric growth to avoid frequent reallocations
 */
void piet_alloc_stack_space(int min_size);

/*
 *  Print stack contents for debugging/tracing
 *  Shows all values from bottom to top when trace is enabled
 *  Only outputs when trace is enabled and tprintf() is active
 */
void piet_dump_stack(void);

/*
 *  Free all stack memory and reset stack state
 *  Called at program termination to clean up resources
 */
void piet_stack_cleanup(void);

#endif //PIET_STACK_H
