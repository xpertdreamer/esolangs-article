#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "piet_common.h"
#include "piet_color.h"
#include "piet_cell.h"
#include "piet_stack.h"
#include "piet_io.h"
#include "piet_interpreter.h"

/*
 * Global configuration variable definition:
 *
 * verbose Enable verbose informational output (default: disabled)
 * quiet Enable quiet mode - suppresses input prompts (default: disabled)
 * trace Enable execution tracing (default: disabled)
 * debug Enable debug output (default: disabled)
 * max_exec_step Maximum execution steps before forced termination (0 = unlimited)
 * unknown_color Unknown color handling policy
 *               1 = treat as white (default), 0 = treat as black, -1 = error
 * codel_size Codel size for input image (-1 = auto-detect, >0 = specified size)
 * input_filename Input filename provided by user
 * exec_step Current execution step counter (starts at 0)
 * trace_start First execution step to include in trace output
 * trace_end Last execution step to include in trace output
 *           Large number (effectively unlimited)
 */

int verbose = 0;
int quiet = 0;
int trace = 0;
int debug = 0;
unsigned max_exec_step = 0;
int unknown_color = 1;
int codel_size = -1;
char *input_filename = NULL;
unsigned exec_step = 0;
unsigned trace_start = 0;
unsigned trace_end = 1 << 31;

/*
 * Display program usage information
 * program_name Name of the program (argv[0])
 * Shows available options, version, and basic usage examples
 */
void display_usage(const char *program_name) {
    fprintf(stderr, "Piet-interp %s - Piet programming language interpreter\n", PIET_VERSION);
    fprintf(stderr, "%s\n\n", PIET_AUTHOR);

    fprintf(stderr, "Usage: %s [options] <filename.png|filename.ppm>\n\n", program_name);

    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help           Show this help message and exit\n");
    fprintf(stderr, "  -v                   Verbose mode (show informational messages)\n");
    fprintf(stderr, "  -q                   Quiet mode (suppress input prompts)\n");
    fprintf(stderr, "  -t                   Trace execution (show each step)\n");
    fprintf(stderr, "  -d                   Debug mode (show detailed debug information)\n");
    fprintf(stderr, "  -e <steps>           Maximum execution steps (0 = unlimited)\n");
    fprintf(stderr, "  -uu                  Unknown colors cause error (default: treat as white)\n");
    fprintf(stderr, "  -ub                  Unknown colors treated as black\n");
    fprintf(stderr, "  -c <size>            Codel size in pixels (-1 = auto-detect, default)\n");
    fprintf(stderr, "  -ts <step>           Start tracing at specified step\n");
    fprintf(stderr, "  -te <step>           Stop tracing at specified step\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "File formats supported:\n");
    fprintf(stderr, "  PNG  - Recommended format (24-bit RGB, no transparency)\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s -t hello.png      # Run with execution trace\n", program_name);
    fprintf(stderr, "  %s -q program.png    # Run quietly (no prompts)\n", program_name);
    fprintf(stderr, "  %s -e 1000 test.png  # Limit to 1000 execution steps\n", program_name);
    fprintf(stderr, "\n");
}

/*
 * Parse command line arguments and set configuration
 * argc Number of command line arguments
 * argv Array of argument strings
 * Return 0 on success, -1 on error (invalid arguments)
 * Supports all standard npiet options except GD-related features
 */
int parse_arguments(int argc, char **argv) {
    /*
     *  Docs:
     *  opt Current option character
     *  endptr For strtol error checking
     *  value Temporary value storage
     *  optstring Definitions of the short options string for getopt
     *            Options with required arguments are followed by ':'s
     */
    int i;
    char *endptr;
    long value;

    // First pass: handle special combined options that getopt doesn't handle well
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-uu") == 0) {
            unknown_color = -1;
            vprintf("info: unknown colors will cause errors (-uu)\n");
            // Remove this argument by shifting the rest
            for (int j = i; j < argc - 1; j++) {
                argv[j] = argv[j + 1];
            }
            argc--;
            i--;
        } else if (strcmp(argv[i], "-ub") == 0) {
            unknown_color = 0;
            vprintf("info: unknown colors treated as black (-ub)\n");
            // Remove this argument by shifting the rest
            for (int j = i; j < argc - 1; j++) {
                argv[j] = argv[j + 1];
            }
            argc--;
            i--;
        } else if (strncmp(argv[i], "-c", 2) == 0 && strlen(argv[i]) > 2 && isdigit(argv[i][2])) {
            // Handle -c<value> format (e.g., -c5, -c-1)
            value = strtol(argv[i] + 2, &endptr, 10);
            if (*endptr != '\0' || value < -1) {
                eprintf("error: invalid value for -c: %s\n", argv[i] + 2);
                return -1;
            }
            codel_size = (int)value;
            if (codel_size == 0) {
                eprintf("warning: codel size 0 is invalid, using 1\n");
                codel_size = 1;
            }
            vprintf("info: codel size set to %d\n", codel_size);

            // Remove this argument by shifting the rest
            for (int j = i; j < argc - 1; j++) {
                argv[j] = argv[j + 1];
            }
            argc--;
            i--;
        }
    }

    // Now parse remaining arguments with getopt
    const char *optstring = "hvqtde:c:ts:te:";
    int opt;
    optind = 1; // Reset optind

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'h': {
                display_usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            }

            case 'v': {
                verbose++;
                vprintf("info: verbose output enabled (level %d)\n", verbose);
                break;
            }

            case 'q': {
                quiet++;
                vprintf("info: quiet mode enabled (no input prompts)\n");
                break;
            }

            case 't': {
                trace++;
                vprintf("info: execution trace enabled (level %d)\n", trace);
                break;
            }

            case 'd': {
                debug++;
                dprintf("debug: debug output enabled (level %d)\n", debug);
                break;
            }

            case 'e': {
                value = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || value < 0) {
                    eprintf("error: invalid value for -e: %s\n", optarg);
                    return -1;
                }
                max_exec_step = (unsigned)value;
                vprintf("info: maximum execution steps: %u\n", max_exec_step);
                break;
            }

            case 'c': {
                // Handle -c <size>
                value = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || value < -1) {
                    eprintf("error: invalid value for -c: %s\n", optarg);
                    return -1;
                }
                codel_size = (int)value;
                if (codel_size == 0) {
                    eprintf("warning: codel size 0 is invalid, using 1\n");
                    codel_size = 1;
                }
                vprintf("info: codel size set to %d\n", codel_size);
                break;
            }

            case 's': {
                // -ts <step> (trace start)
                value = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || value < 0) {
                    eprintf("error: invalid value for -ts: %s\n", optarg);
                    return -1;
                }
                trace_start = (unsigned)value;
                vprintf("info: trace starts at step %u\n", trace_start);
                break;
            }

            case '?':
                // getopt already printed an error message
                return -1;

            default:
                eprintf("error: unhandled option: -%c\n", opt);
                return -1;
        }
    }

    // optind now points to first non-option argument
    if (optind < argc) {
        input_filename = argv[optind];
        vprintf("info: input file: %s\n", input_filename);
        optind++;

        // Check for extra arguments (should only be one filename)
        if (optind < argc) {
            eprintf("error: extra argument: %s\n", argv[optind]);
            return -1;
        }
    } else {
        eprintf("error: no input file specified\n");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int result;

    if (parse_arguments(argc, argv) < 0) {
        display_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    vprintf("info: loading Piet program from '%s'\n", input_filename);
    result = piet_read_png(input_filename);
    if (result < 0) {
        eprintf("error: could not load Piet program from '%s'\n", input_filename);
        exit(EXIT_FAILURE);
    }

    if (codel_size != 1) {
        vprintf("info: processing codel size\n");
        piet_cleanup_input();
    }

    if (debug) {
        piet_dump_cells();
    }

    vprintf("info: starting Piet program execution\n");
    result = piet_run();
    vprintf("info: program execution completed\n");

    piet_stack_cleanup();
    if (cells != NULL) {
        free(cells);
        cells = NULL;
        width = height = 0;
    }
    if (result < 0) {
        eprintf("error: program terminated with error\n");
        return EXIT_FAILURE;
    }

    vprintf("info: program terminated successfully\n");
    return EXIT_SUCCESS;
}