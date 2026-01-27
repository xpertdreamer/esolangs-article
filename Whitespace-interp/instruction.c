//
// Created by IWOFLEUR on 25.01.2026.
//

#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>

// ARITHMETIC OPERATIONS

void instr_add(Interpreter* interpreter) {
    if (interpreter->stack->top < 1) {
        fprintf(stderr, "Add: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);

    // Overflow check
    long long result = (long long)a + (long long)b;
    if (result > INT_MAX || result < INT_MIN) {
        fprintf(stderr, "Add: integer overflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    st_push(interpreter->stack, a + b);
}

void instr_sub(Interpreter* interpreter) {
    if (interpreter->stack->top < 1) {
        fprintf(stderr, "Sub: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);

    st_push(interpreter->stack, a - b);
}

void instr_mul(Interpreter* interpreter) {
    if (interpreter->stack->top < 1) {
        fprintf(stderr, "Mul: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);

    // Overflow check
    if (a != 0 && b != 0) {
        long long result = (long long)a * (long long)b;
        if (result > INT_MAX || result < INT_MIN) {
            fprintf(stderr, "Mul: integer overflow at line %d\n", interpreter->parser.line);
            interpreter->running = false;
            return;
        }
    }

    st_push(interpreter->stack, a * b);
}

void instr_div(Interpreter* interpreter) {
    if (interpreter->stack->top < 1) {
        fprintf(stderr, "Div: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);

    if (b == 0) {
        fprintf(stderr, "Div: divide by zero at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    st_push(interpreter->stack, a / b);
}

void instr_mod(Interpreter* interpreter) {
    if (interpreter->stack->top < 1) {
        fprintf(stderr, "Mod: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int b = st_pop(interpreter->stack);
    int a = st_pop(interpreter->stack);

    if (b == 0) {
        fprintf(stderr, "Mod: modulo by zero at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    st_push(interpreter->stack, a % b);
}

// HEAP OPERATIONS

void instr_heap_store(Interpreter* interpreter) {
    if (interpreter->stack->top < 1) {
        fprintf(stderr, "Heap store: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int value = st_pop(interpreter->stack);
    int address = st_pop(interpreter->stack);

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "Heap store: address %d out of bounds [0, %d) at line %d\n",
                address, HEAP_SIZE, interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    interpreter->heap[address] = value;
}

void instr_heap_retrieve(Interpreter* interpreter) {
    if (interpreter->stack->top < 0) {
        fprintf(stderr, "Heap retrieve: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int address = st_pop(interpreter->stack);

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "Heap retrieve: address %d out of bounds [0, %d) at line %d\n",
                address, HEAP_SIZE, interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    st_push(interpreter->stack, interpreter->heap[address]);
}

// I/O OPERATIONS

void instr_out_char(Interpreter *interpreter) {
    if (interpreter->stack->top < 0) {
        fprintf(stderr, "Out char: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int value = st_pop(interpreter->stack);

#ifdef DEBUG
    printf("\n[DEBUG] Out char: ");
    if (value > 32 && value <= 126) {
        printf("'%c' (ASCII %d)\n", value, value);
    } else if (value == 10) {
        printf("newline '\\n'\n");
    } else if (value == 9) {
        printf("tab '\\t'\n");
    } else if (value == 32) {
        printf("space\n");
    } else if (value == 0) {
        printf("NULL character\n");
    } else {
        printf("control character (ASCII %d)\n", value);
    }
#endif

    putchar(value);
    fflush(stdout);
}

void instr_out_num(Interpreter* interpreter) {
    if (interpreter->stack->top < 0) {
        fprintf(stderr, "Out num: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int value = st_pop(interpreter->stack);

#ifdef DEBUG
    printf("\n[DEBUG] Out num: %d\n", value);
#endif

    printf("%d", value);
    fflush(stdout);
}

void instr_in_char(Interpreter* interpreter) {
    if (interpreter->stack->top < 0) {
        fprintf(stderr, "In char: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int address = st_pop(interpreter->stack);

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "In char: address %d out of bounds [0, %d) at line %d\n",
                address, HEAP_SIZE, interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    fflush(stdout);
    fflush(stderr);

    int c = getchar();

    // Skip whitespace
    while (c == ' ' || c == '\t' || c == '\n') {
        c = getchar();
    }

    if (c == EOF) {
        c = -1;
    }

    interpreter->heap[address] = c;
}

void instr_in_num(Interpreter* interpreter) {
    if (interpreter->stack->top < 0) {
        fprintf(stderr, "In num: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int address = st_pop(interpreter->stack);

    if (address < 0 || address >= HEAP_SIZE) {
        fprintf(stderr, "In num: address %d out of bounds [0, %d) at line %d\n",
                address, HEAP_SIZE, interpreter->parser.line);
        interpreter->running = false;
        return;
    }

#ifdef DEBUG
    printf("[DEBUG] Reading number input: ");
#endif

    fflush(stdout);
    fflush(stderr);

    char buffer[32] = {0};
    int c;

    // Skip leading whitespace (spaces and tabs only)
    do {
        c = getchar();
        if (c == EOF) {
            interpreter->heap[address] = 0;
            return;
        }
    } while (c == ' ' || c == '\t');

    // Empty line = 0
    if (c == '\n') {
        interpreter->heap[address] = 0;
        return;
    }

    // Build number string
    buffer[0] = (char)c;
    int i = 1;

    while (i < 31) {
        c = getchar();
        if (c == EOF || c == '\n' || c == ' ' || c == '\t') {
            break;
        }
        buffer[i++] = (char)c;
    }
    buffer[i] = '\0';

    // Parse the number
    int value = 0;
    int sign = 1;
    int j = 0;

    if (buffer[0] == '-') {
        sign = -1;
        j = 1;
    } else if (buffer[0] == '+') {
        j = 1;
    }

    for (; buffer[j] != '\0'; j++) {
        if (buffer[j] >= '0' && buffer[j] <= '9') {
            value = value * 10 + (buffer[j] - '0');
        } else {
            // Invalid character in number
            break;
        }
    }

#ifdef DEBUG
    printf("%d\n", sign * value);
#endif

    interpreter->heap[address] = sign * value;

    // Consume rest of line
    if (c != '\n' && c != EOF) {
        while ((c = getchar()) != '\n' && c != EOF) {
            // Skip to end of line
        }
    }
}

// FLOW CONTROL HELPERS

void fc_add_label(Interpreter* interpreter, int label, int position) {
    if (interpreter->label_count >= MAX_LABELS) {
        fprintf(stderr, "Too many labels (max %d) at line %d\n",
                MAX_LABELS, interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    // Check if label already exists (update position)
    for (int i = 0; i < interpreter->label_count; i++) {
        if (interpreter->labels[i].address == label) {
            interpreter->labels[i].position = position;
            return;
        }
    }

    // Add new label
    interpreter->labels[interpreter->label_count].address = label;
    interpreter->labels[interpreter->label_count].position = position;
    interpreter->label_count++;
}

// Returns position if found, -1 if not found
int fc_find_label(Interpreter* interpreter, int label) {
    for (int i = 0; i < interpreter->label_count; i++) {
        if (interpreter->labels[i].address == label) {
            return interpreter->labels[i].position;
        }
    }

    fprintf(stderr, "Undefined label: %d at line %d\n", label, interpreter->parser.line);
    interpreter->running = false;
    return -1;
}

// FLOW CONTROL INSTRUCTIONS

void instr_mark(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    fc_add_label(interpreter, label, interpreter->parser.position);
    // Mark is just a label declaration - handled during label collection
    // No runtime action needed
}

void instr_call_subroutine(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);

    if (target < 0) {
        // Error already reported, running = false
        return;
    }

    if (interpreter->call_stack->top >= CALL_STACK_SIZE - 1) {
        fprintf(stderr, "Call stack overflow (max %d) at line %d\n",
                CALL_STACK_SIZE, interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    // Push return address and jump
    interpreter->call_stack->data[++interpreter->call_stack->top] = interpreter->parser.position;
    interpreter->parser.position = target;
}

void instr_jump(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);

    if (target < 0) {
        return;
    }

    interpreter->parser.position = target;
}

void instr_jump_if_zero(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);

    if (target < 0) {
        return;
    }

    if (interpreter->stack->top < 0) {
        fprintf(stderr, "Jump if zero: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int value = st_pop(interpreter->stack);

    if (value == 0) {
        interpreter->parser.position = target;
    }
}

void instr_jump_if_neg(Interpreter* interpreter) {
    int label = parse_label(&interpreter->parser);
    int target = fc_find_label(interpreter, label);

    if (target < 0) {
        return;
    }

    if (interpreter->stack->top < 0) {
        fprintf(stderr, "Jump if negative: stack underflow at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    int value = st_pop(interpreter->stack);

    if (value < 0) {
        interpreter->parser.position = target;
    }
}

void instr_ret(Interpreter* interpreter) {
    if (interpreter->call_stack->top < 0) {
        fprintf(stderr, "Return with empty call stack at line %d\n", interpreter->parser.line);
        interpreter->running = false;
        return;
    }

    // Pop return address and jump back
    int ret_pos = interpreter->call_stack->data[interpreter->call_stack->top--];
    interpreter->parser.position = ret_pos;
}

void instr_end(Interpreter* interpreter) {
    interpreter->running = false;
}