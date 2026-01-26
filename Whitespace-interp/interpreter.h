//
// Created by IWOFLEUR on 25.01.2026.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

// Global options
#define HEAP_SIZE 524228
#define STACK_SIZE 65536
#define BUF_SIZE 4096
#define MAX_LABELS 1024
#define CALL_STACK_SIZE 256

// Lexical tokens
#define SPACE ' '
#define TAB '\t'
#define LINEFEED '\n'
#define NULL_TERM '\0'

#include <stdbool.h>

typedef struct {
    char* source;       // Source file
    long long length;      // Length of source code
    int position;       // Current position
    int line;           // Current line on interp
    int col;            // Same as line but with a column
} ParserState;

typedef struct {
    int* data;
    int top;
    int capacity;
} Stack;

typedef struct {
    int address;        // Address of the label (decimal)
    int position;       // Position in the code
} Label;

typedef struct {
    Stack* stack;       // Value stack
    int *heap;          // Heap
    Label* labels;      // Array of labels
    int label_count;
    Stack* call_stack;
    bool running;
    ParserState parser;
} Interpreter;

Stack* st_new(int capacity);
void st_free(Stack *stack);
bool st_push(Stack *stack, int value);
int st_pop(Stack *stack);
int st_peek(Stack *stack, int offset);

char parse_next_char(ParserState *parser);
char parse_peek_char(ParserState *parser);
void parse_skip_ws(ParserState *parser);
int parse_number(ParserState *parser);
int parse_label(ParserState *parser);

Interpreter* interpreter_new(void);
void interpreter_delete(Interpreter* interpreter);
int interpreter_load_str(Interpreter* interpreter, const char* source);
int interpreter_read_from_file(Interpreter* interpreter, const char *source);
void interpreter_run(Interpreter* interpreter);

#endif //INTERPRETER_H
