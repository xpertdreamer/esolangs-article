//
// Created by IWOFLEUR on 25.01.2026.
//
#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>

// Arithmetics

void instr_add(Interpreter* interpreter) {
    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);
    st_push(interpreter->stack, a + b);
}

void instr_sub(Interpreter* interpreter) {
    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);
    st_push(interpreter->stack, a - b);
}

void instr_mul(Interpreter* interpreter) {
    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);
    st_push(interpreter->stack, a * b);
}

void instr_div(Interpreter* interpreter) {
    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);
    if (b == 0) {
        fprintf(stderr, "Divide by zero at line %d\n", interpreter->parser.line);
        exit(EXIT_FAILURE);
    }

    st_push(interpreter->stack, a / b);
}

void instr_mod(Interpreter* interpreter) {
    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);
    if (b == 0) {
        fprintf(stderr, "Divide by zero at line %d\n", interpreter->parser.line);
        exit(EXIT_FAILURE);
    }

    st_push(interpreter->stack, a % b);
}

// Heap

void instr_heap_store(Interpreter* interpreter) {
    int value = st_pop(interpreter->stack);
    int address = st_pop(interpreter->stack);

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "Address out of bounds at line %d\n", interpreter->parser.line);
        exit(EXIT_FAILURE);
    }

    interpreter->heap[address] = value;
}

void instr_heap_retrieve(Interpreter* interpreter) {
    int address = st_pop(interpreter->stack);

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "Address out of bounds at line %d\n", interpreter->parser.line);
        exit(EXIT_FAILURE);
    }

    st_push(interpreter->stack, interpreter->heap[address]);
}

// IO

void instr_out_char(Interpreter *interpreter) {
    int value = st_pop(interpreter->stack);
    putchar(value);
    fflush(stdout);
}

void instr_out_num(Interpreter* interpreter) {
    int value = st_pop(interpreter->stack);
    printf("%d", value);
    fflush(stdout);
}

void instr_in_char(Interpreter* interpreter) {
    int address = st_pop(interpreter->stack);
    int c = getchar();

    if (c == EOF)
        c = -1;

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "Address out of bounds: %d\n", address);
        exit(EXIT_FAILURE);
    }

    interpreter->heap[address] = c;
}

void instr_in_num(Interpreter* interpreter) {
    int address = st_pop(interpreter->stack);
    int value;

    if (scanf("%d", &value) != 1)
        value = 0;

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "Address out of bounds: %d\n", address);
        exit(EXIT_FAILURE);
    }

    interpreter->heap[address] = value;
}

// Flow control

void fc_add_label(Interpreter* interpreter, int label, int position) {
    if (interpreter->label_count >= MAX_LABELS) {
        fprintf(stderr, "Too many labels at line %d\n", interpreter->parser.line);
        exit(EXIT_FAILURE);
    }

    interpreter->labels[interpreter->label_count].address = label;
    interpreter->labels[interpreter->label_count].position = position;
    interpreter->label_count++;
}

int fc_find_label(Interpreter* interpreter, int label) {
    for (int i = 0; i < interpreter->label_count; i++)
        if (interpreter->labels[i].address == label)
            return interpreter->labels[i].position;

    fprintf(stderr, "Undefined label: %d\n", label);
    exit(EXIT_FAILURE);
}

void instr_mark(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    fc_add_label(interpreter, label, interpreter->parser.position);
}

void instr_call_subroutine(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);

    if (interpreter->call_stack->top >= CALL_STACK_SIZE - 1) {
        fprintf(stderr, "Call stack overflow at line %d\n", interpreter->parser.line);
        exit(EXIT_FAILURE);
    }

    interpreter->call_stack->data[++interpreter->call_stack->top] = interpreter->parser.position;
    interpreter->parser.position = target;
}

void instr_jump(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);
    interpreter->parser.position = target;
}

void instr_jump_if_zero(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);
    int value = st_pop(interpreter->stack);

    if (value == 0) {
        interpreter->parser.position = target;
    }
}

void instr_jump_if_neg(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);
    int value = st_pop(interpreter->stack);

    if (value < 0) {
        interpreter->parser.position = target;
    }
}

void instr_ret(Interpreter* interpreter) {
    if (interpreter->call_stack->top < 0) {
        fprintf(stderr, "Return with empty call stack\n");
        exit(EXIT_FAILURE);
    }

    const int ret_pos = interpreter->call_stack->data[interpreter->call_stack->top--];
    interpreter->parser.position = ret_pos;
}

void instr_end(Interpreter* interpreter) {
    interpreter->running = false;
}