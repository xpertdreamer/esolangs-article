//
// Created by IWOFLEUR on 28.01.2026.
//

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "interpreter.h"
#include "log.h"
#include "png_loader.h"

static void print_help(const char * program_name);

int main(int argc, char** argv) {
    int opt;
    int step_mode = 0;
    struct option long_options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"trace", no_argument, 0, 't'},
        {"debug", no_argument, 0, 'd'},
        {"step", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "vtdsh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v': piet_log_level = PIET_LOG_INFO; break;
            case 't': piet_log_level = PIET_LOG_TRACE; break;
            case 'd': piet_log_level = PIET_LOG_DEBUG; break;
            case 's': step_mode = 1; break;
            case 'h': print_help(argv[0]); return 0;
            default: print_help(argv[0]); return 1;
        }
    }

    if (optind == argc) {
        fprintf(stderr, "Error: No input file specified.\n\n");
        print_help(argv[0]);
        return 1;
    }

    const char* filename = argv[optind];
    PIET_INFO("Loading Piet program: %s\n", filename);

    PietImage *image = piet_load_png(filename);
    if (image == NULL) {
        PIET_ERROR("Failed to load image: %s\n", filename);
        return 1;
    }

    Piet *piet = piet_new(image);
    piet->tracing = (piet_log_level == PIET_LOG_TRACE);
    PIET_INFO("Starting execution...\n");

    if (step_mode) {
        printf("Step mode enabled. Press Enter to step, 'q' to quit.\n");
        while (piet_step(piet)) {
            printf("> ");
            int ch = getchar();
            if (ch == 'Q' || ch == 'q') {
                break;
            }
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
        }
    } else piet_run(piet);

    printf("\n");
    piet_print_state(piet);
    piet_free(piet);
    piet_free_png(image);

    PIET_INFO("Execution completed.\n");
    return 0;
}

static void print_help(const char * program_name) {
    printf("Piet Interpreter - Complete implementation\n");
    printf("Usage: %s [OPTIONS] <image.png>\n", program_name);
    printf("\nOptions:\n");
    printf("  -v, --verbose         Verbose output\n");
    printf("  -t, --trace           Trace execution step-by-step\n");
    printf("  -d, --debug           Debug mode (very verbose)\n");
    printf("  -s, --step            Step mode (pause after each command)\n");
    printf("  -h, --help            Show this help\n");
    printf("\nExamples:\n");
    printf("  %s -v program.png       Run with verbose output\n", program_name);
    printf("  %s -t program.png       Trace each execution step\n", program_name);
}