//
// Created by IWOFLEUR on 28.01.2026.
//

#ifndef STACK_H
#define STACK_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Stack {
    int *data;
    int capacity;
    int top;
} Stack;

// Stack API
Stack* st_create(int capacity);
void st_destroy(Stack *stack);
bool st_is_empty(const Stack *stack);
bool st_push(Stack *stack, int value);
int st_pop(Stack *stack);
int st_peek(const Stack *stack);
int st_size(const Stack *stack);
void st_clear(Stack *stack);
void st_print(const Stack *stack);

#endif //STACK_H
