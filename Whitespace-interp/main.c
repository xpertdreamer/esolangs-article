#include "interpreter.h"
#include <stdio.h>
#include <getopt.h>

void print_version(void);
void print_help(const char* program_name);
void create_test_program(const char* filename);
// void dump_file(const char *filename);

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
        // dump_file("test.ws");
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
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Cannot create test file");
        return;
    }

    /* Prints "Hello, World!\n"
     *
     * Structure of each symbol:
     * 1. Push ASCII code of symbol
     * 2. Output char
     *
     * End program
     */

    // String to output
    const char *str = "Hello, World!\n";

    for (int i = 0; str[i] != '\0'; i++) {
        int ch = str[i];

        // 1. IMP Stack + Push
        fputc(' ', file);  // IMP: Stack
        fputc(' ', file);  // Operation: Push

        // 2. Sign
        fputc(' ', file);  // Positive sign

        // 3. ASCII code (7 bit for ASCII)
        int value = ch;
        int bits[32] = {0};
        int bit_count = 0;

        // Convert to binary without leading zeros
        while (value > 0) {
            bits[bit_count++] = value & 1;
            value >>= 1;
        }

        // Write the bits from the most significant to the least significant
        // If bit_count == 0 (digit 0), write one zero char
        if (bit_count == 0) {
            fputc(' ', file);  // 0
        } else {
            for (int j = bit_count - 1; j >= 0; j--) {
                fputc(bits[j] ? '\t' : ' ', file);
            }
        }

        // 4. End of digit
        fputc('\n', file);

        // 5. Output char
        fputc('\t', file);  // IMP: I/O
        fputc('\n', file);
        fputc(' ', file);   // Operation: Output char
        fputc(' ', file);
    }

    // End program
    fputc('\n', file);
    fputc('\n', file);
    fputc('\n', file);

    fclose(file);
    printf("Created Hello World program: %s\n", filename);
    printf("Will print: %s", str);
}
//
// void dump_file(const char *filename) {
//     FILE *f = fopen(filename, "rb");
//     if (!f) return;
//
//     printf("Dumping file %s:\n", filename);
//     int pos = 0;
//     int ch;
//     while ((ch = fgetc(f)) != EOF) {
//         printf("%3d: ", pos++);
//         if (ch == ' ') {
//             printf("SPACE\n");
//         } else if (ch == '\t') {
//             printf("TAB\n");
//         } else if (ch == '\n') {
//             printf("LF\\n\n");
//         } else {
//             printf("'%c' (ASCII %d)\n", ch, ch);
//         }
//     }
//     fclose(f);
//     printf("\n");
// }

