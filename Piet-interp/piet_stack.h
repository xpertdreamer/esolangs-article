//
// Created by IWOFLEUR on 01.02.2026.
//
#pragma once

#ifndef PIET_STACK_H
#define PIET_STACK_H

#include "piet_types.h"

typedef struct {
    long* data;
    int num_stack;
    int max_stack;
} Stack;

// Stack function prototypes
Stack* stack_create(int initial_size);          // Create new stack with given initial size
void stack_free(Stack *stack);                  // Free all memory used by stack
void stack_resize(Stack *stack, int new_size);  // Resize stack to new capacity
void stack_push(Stack *stack, long value);      // Push value onto stack
long stack_pop(Stack *stack);                   // Pop value from stack
long stack_peek(const Stack *stack);                  // Peek at top value without popping
int stack_size(Stack *stack);                   // Get current stack size
void stack_dump(Stack *stack);                  // Print stack contents for debugging

#endif //PIET_STACK_H
