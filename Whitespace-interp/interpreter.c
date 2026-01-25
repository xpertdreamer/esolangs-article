//
// Created by IWOFLEUR on 25.01.2026.
//
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

#include <string.h>

Stack* st_new(const int capacity) {
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        return NULL;
    }

    stack->data = (int *)malloc(sizeof(int) * capacity);
    if (stack->data == NULL) {
        free(stack);
        return NULL;
    }

    stack->capacity = capacity;
    stack->top = -1;

    return stack;
}

void st_free(Stack *stack) {
    free(stack->data);
    free(stack);
}

bool st_push(Stack *stack, const int value) {
    if (stack->top >= stack->capacity - 1) {
        fprintf(stderr, "Stack overflow\n");
        return false;
    }

    stack->data[++stack->top] = value;
    return true;
}

int st_pop(Stack *stack) {
    if (stack->top < 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(EXIT_FAILURE);
    }

    return stack->data[stack->top--];
}

int st_peek(Stack *stack, const int offset) {
    if (stack->top - offset < 0) {
        fprintf(stderr, "Stack: Out of bounds\n");
        exit(EXIT_FAILURE);
    }

    return stack->data[stack->top - offset];
}

Interpreter* interpreter_new(void) {
    Interpreter *interpreter = (Interpreter *)malloc(sizeof(Interpreter));

    interpreter->stack = st_new(STACK_SIZE);
    interpreter->heap = calloc(HEAP_SIZE, sizeof(int));
    interpreter->labels = malloc(MAX_LABELS * sizeof(Label));
    interpreter->label_count = 0;
    interpreter->call_stack = st_new(CALL_STACK_SIZE);
    interpreter->running = true;

    interpreter->parser.source = NULL;
    interpreter->parser.length = 0;
    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;

    return interpreter;
}

void interpreter_free(Interpreter *interpreter) {
    if (interpreter == NULL) return;

    st_free(interpreter->stack);
    free(interpreter->heap);
    free(interpreter->labels);
    st_free(interpreter->call_stack);
    free(interpreter->parser.source);
    free(interpreter);
}

int interpreter_read_from_file(Interpreter* interpreter, const char* source) {
    FILE* file = fopen(source, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    interpreter->parser.source = (char*)malloc(size + 1);
    if (interpreter->parser.source == NULL) {
        fclose(file);
        perror("Error allocating memory");
        return -1;
    }

    fread(interpreter->parser.source, 1, size, file);
    interpreter->parser.source[size] = NULL_TERM;
    interpreter->parser.length = size;
    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;

    fclose(file);
    return 0;
}

int interpreter_load_str(Interpreter* interpreter, const char* source) {
    const size_t size = strlen(source);
    interpreter->parser.source = (char*)malloc(size + 1);
    if (interpreter->parser.source == NULL)
        return -1;

    strcpy(interpreter->parser.source, source);
    interpreter->parser.length = size;
    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;

    return 0;
}

char parse_next_char(ParserState *parser) {
    if (parser->position >= parser->length) {
        return EOF;
    }

    char c = parser->source[parser->position++];

    if (c == LINEFEED) {
        parser->line++;
        parser->col = 1;
    } else
        parser->col++;

    return c;
}

char parse_peek_char(ParserState *parser) {
    if (parser->position >= parser->length) {
        return EOF;
    }

    return parser->source[parser->position];
}

void parse_skip_ws(ParserState *parser) {
    while (parser->position < parser->length) {
        const char c = parser->source[parser->position];
        if (c != SPACE && c != TAB && c != LINEFEED)
            parser->position++;
        else
            break;
    }
}

int parse_number(ParserState *parser) {
    char c = parse_next_char(parser);
    int sign = 1;

    if (c == TAB)
        sign = -1;
    else if (c != SPACE) {
        fprintf(stderr, "Unexpected character %c at %d %d" ,c ,parser->line ,parser->col);
        exit(EXIT_FAILURE);
    }

    int value = 0;
    while ((c = parse_next_char(parser)) != LINEFEED) {
        value <<= 1;
        if (c == TAB)
            value |= 1;
        else if (c != SPACE) {
            fprintf(stderr, "Invalid binary digit at %d %d" ,parser->line ,parser->col);
            exit(EXIT_FAILURE);
        }
    }

    return sign * value;
}

int parse_label(ParserState *parser) {
    int label = 0;
    char c;

    while ((c = parse_next_char(parser)) != LINEFEED) {
        label <<= 1;
        if (c == TAB)
            label |= 1;
        else if (c != SPACE) {
            fprintf(stderr, "Invalid label character at %d %d" ,parser->line ,parser->col);
            exit(EXIT_FAILURE);
        }
    }

    return label;
}

void instruction_push(Interpreter* interpreter) {
    int value = parse_number(&interpreter->parser);
    st_push(interpreter->stack, value);
}

void instruction_duplicate(Interpreter* interpreter) {
    int value = st_peek(interpreter->stack, 0);
    st_push(interpreter->stack, value);
}

void instruction_swap(Interpreter* interpreter) {
    int a = st_pop(interpreter->stack);
    int b = st_pop(interpreter->stack);
    st_push(interpreter->stack, a);
    st_push(interpreter->stack, b);
}

void instruction_discard(Interpreter* interpreter) {
    st_pop(interpreter->stack);
}