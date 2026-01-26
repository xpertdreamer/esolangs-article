#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void print_version(void);
void print_help(const char* program_name);
void create_test_program(const char* filename);

int main(const int argc, char** argv) {
    bool execute_directly = false;
    const char* filename = NULL;
    const char* direct_code = NULL;

    const struct option long_options[] = {
        {"help",    no_argument,        0, 'h'},
        {"execute", required_argument,  0, 'e'},
        {"version", no_argument,        0, 'v'},
        {0,         0,                  0,  0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "he:v", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                return 0;

            case 'v':
                print_version();
                return 0;

            case 'e':
                execute_directly = true;
                direct_code = optarg;
                break;

            default:
                print_help(argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        filename = argv[optind];
    }

    if (execute_directly && filename) {
        fprintf(stderr, "Error: Cannot specify both -e and file\n");
        print_help(argv[0]);
        return 1;
    }

    Interpreter* interpreter = interpreter_new();
    if (interpreter == NULL) {
        fprintf(stderr, "Error creating interpreter\n");
        return 1;
    }

    int load_res = 0;
    if (execute_directly) {
        load_res = interpreter_load_str(interpreter, direct_code);
    } else if (filename) {
        load_res = interpreter_read_from_file(interpreter, filename);
    } else {
        create_test_program("test.ws");
        load_res = interpreter_read_from_file(interpreter, "test.ws");
    }

    if (load_res != 0) {
        fprintf(stderr, "Error loading program\n");
        interpreter_delete(interpreter);
        return 1;
    }

    interpreter_run(interpreter);


    interpreter_delete(interpreter);
    return 0;
}

void print_version(void) {
    printf("Whitespace-interpreter v0.1\n");
    printf("Implementation of every Whitespace instruction\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
}

void print_help(const char* program_name) {
    printf("Whitespace-interpreter v0.1\n");
    printf("Usage: %s [options] <file.ws>\n\n", program_name);
    printf("Options:\n");
    printf("    -h                      Print this help.\n");
    printf("    -e                      Execute line directly\n");
    printf("Examples:\n");
    printf("    %s program.ws           Execute from file\n", program_name);
    printf("    %s -e \"   \\t\\n\"     Execute inline code\n", program_name);
}

void create_test_program(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Simple program: prints "Hi"
    /* Push 72 (H), output char, push 105 (i), output char, end */
    fprintf(file, "   \t \t \n");      // Push 72
    fprintf(file, "\t\n  \t \n");      // Output char
    fprintf(file, "   \t \t\t \n");    // Push 105
    fprintf(file, "\t\n  \t \n");      // Output char
    fprintf(file, "\n\n\n");           // End program

    fclose(file);
    printf("Created test program: %s\n", filename);
}