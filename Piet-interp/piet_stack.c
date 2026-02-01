//
// Created by IWOFLEUR on 01.02.2026.
//

#include "piet_stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new stack with specified initial size
Stack* create_stack(int initial_size) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (stack == NULL) return NULL;

    stack->data = (long*)calloc(initial_size, sizeof(long));
    stack->num_stack = 0;
    stack->max_stack = initial_size;
    return stack;
}

// Free all memory associated with stack
void stack_free(Stack *stack) {
    if (stack != NULL) {
        free(stack->data);
        free(stack);
    }
}

// Resize stack to new capacity
void stack_resize(Stack *stack, int new_size) {
    if (new_size <= stack->max_stack) return;

    long* new_data = (long*)calloc(new_size, sizeof(long));
    if (new_data != NULL) {
        // Copy existing data to new array
        memcpy(new_data, stack->data, sizeof(long) * stack->num_stack);
        free(stack->data);
        stack->data = new_data;
        stack->max_stack = new_size;
    }
}

// Push value onto stack (using macro for efficiency)
void stack_push(Stack *stack, long value) {
    STACK_PUSH(stack, value);
}

// Pop value from stack (using macro for efficiency)
long stack_pop(Stack *stack) {
    return STACK_POP(stack);
}

// Peek at top value without removing it (using macro for efficiency)
long stack_peek(const Stack *stack) {
    return STACK_PEEK(stack);
}

// Get current number of elements in stack
int stack_size(const Stack *stack) {
    return stack->num_stack;
}

// Print stack contents for debugging/tracing
void stack_dump(Stack *stack) {
    if (stack ->num_stack == 0)
        TPRINTF("[TRACE] Stack is empty");
    else {
        TPRINTF("[TRACE] Stack (%d values):", stack->num_stack);
        // Print all values from bottom to top
        for (int i = 0; i < stack->num_stack; i++) {
            TPRINTF(" %ld", stack->data[stack->num_stack - i - 1]);
        }
    }
    TPRINTF("\n");
}