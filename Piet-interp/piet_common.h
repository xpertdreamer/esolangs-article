//
// Created by IWOFLEUR on 03.02.2026.
//

#ifndef PIET_COMMON_H
#define PIET_COMMON_H

#define PIET_VERSION "0.0.1"
#define PIET_AUTHOR "Based on npiet by Erik Schoenfelder"

/* Here some debug
 * and output control macros
 * down there */

// When commented out, all debug output is removed at compile time
#define DEBUG_ENABLED

// Uncomment to enable execution trace output macros
#define TRACE_ENABLED

#ifdef DEBUG_ENABLED
// Print debug messages when debug flag is non-zero
#define dprintf(...) do { if (debug) printf(__VA_ARGS__); } while(0)
// Print verbose debug messages when debug flag > 1
#define d2printf(...)   do { if (debug > 1) printf(__VA_ARGS__); } while(0)
#else
// When DEBUG_ENABLED is not defined, debug macros expand to nothing
#define dprintf(...)    ((void)0)
#define d2printf(...)   ((void)0)
#endif

#ifdef TRACE_ENABLED
// Print trace messages when trace is enabled and step is in range
#define tprintf(...)    do { if (trace && exec_step >= trace_start && \
                        exec_step <= trace_end) printf(__VA_ARGS__); } while(0)
// Print detailed trace messages when trace > 1
#define t2printf(...)   do { if (trace > 1) printf(__VA_ARGS__); } while(0)
#else
// When TRACE_ENABLED is not defined, trace macros expand to nothing
    #define tprintf(...)    ((void)0)
    #define t2printf(...)   ((void)0)
#endif

// Print verbose informational messages
#define vprintf(...)    do { if (verbose) printf(__VA_ARGS__); } while(0)

// Print error messages to stderr
#define eprintf(...)    do { fprintf(stderr, __VA_ARGS__); } while(0)

/*
 * Some mathematical helpers here
 */

// Calculate absolute value of x
#define ABS(x) ((x) < 0 ? -(x) : (x))

// Get max of two values
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// Get min of two values
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

// Get sign of value (-1, 0, 1)
#define SIGN(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)

/*
 * Memory managment macros
 */

/*
 *  Safe memory allocation with error checking
 *  ptr Pointer variable to allocate
 *  type Data type of elements
 *  count Number of elements to allocate
 *  Exits program with error message if allocation fails
 */
#define SAFE_ALLOC(ptr, type, count) \
    do { \
        (ptr) = (type *)calloc((count), sizeof(type)); \
        if (!(ptr)) { \
            eprintf("fatal: memory allocation failed for " #ptr " (%d bytes)\n", \
                    (int)((count) * sizeof(type))); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)


/*
 *  Safe memory reallocation with error checking
 *  ptr Pointer variable to reallocate
 *  type Data type of elements
 *  count New number of elements
 *  Exits program with error message if reallocation fails
 */
#define SAFE_REALLOC(ptr, type, count) \
    do { \
        (type)* temp = (type *)realloc((ptr), (count) * sizeof(type)); \
        if (!(temp)) { \
            eprintf("fatal: memory reallocation failed for " #ptr " (%d bytes)\n", \
                    (int)((count) * sizeof(type))); \
            free((ptr)); \
            exit(EXIT_FAILURE); \
        } \
        (ptr) = temp; \
    } while (0)

/*
 *  Global conviguration variables
 */

// Enable verbose output mode (non-zero = enabled)
extern int verbose;

// Enable quiet mode - suppresses input prompts (non-zero = enabled)
extern int quiet;

// Enable execution tracing (non-zero = enabled, higher = more detail)
extern int trace;

// Enable debug output (non-zero = enabled, higher = more detail)
extern int debug;

// Maximum execution steps before termination (0 = unlimited)
extern unsigned max_exec_step;

/*
 *  How to handle unknown colors in input images
 *  1 = treat as white (default), 0 = treat as black, -1 = error
 */
extern int unknown_color;

/*
 *  Size of each codel in input image pixels
 *  -1 = guess automatically, positive = specified size
 */
extern int codel_size;

// Input filename provided by user
extern char *input_filename;

// Current execution step counter
extern int exec_step;

// First execution step to include in trace output
extern unsigned trace_start;

// Last execution step to include in trace output
extern unsigned trace_end;

#endif //PIET_COMMON_H