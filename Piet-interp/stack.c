//
// Created by IWOFLEUR on 28.01.2026.
//

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

Stack* st_create(int capacity) {
    Stack *stack = (Stack*)malloc(sizeof(Stack));
    if (stack == NULL) return NULL;

    stack->data = (int*)malloc(sizeof(int) * capacity);
    if (stack->data == NULL) {
        free(stack);
        return NULL;
    }

    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

void st_destroy(Stack *stack) {
    if (stack) {
        free(stack->data);
        free(stack);
    }
}

bool st_is_empty(const Stack *stack) {
    return stack->top == -1;
}

bool st_push(Stack *stack, int value) {
    if (stack->top >= stack->capacity - 1) {
        int new_capacity = stack->capacity * 2;
        int *new_data = (int*)realloc(stack->data, sizeof(int) * new_capacity);
        if (new_data == NULL) return false;

        stack->data = new_data;
        stack->capacity = new_capacity;
    }

    stack->data[++stack->top] = value;
    return true;
}

int st_pop(Stack *stack) {
    if (st_is_empty(stack)) return 0;
    return stack->data[stack->top--];
}

int st_peek(const Stack *stack) {
    if (st_is_empty(stack)) return 0;
    return stack->data[stack->top];
}

int st_size(const Stack *stack) {
    return stack->top + 1;
}

void st_clear(Stack *stack) {
    stack->top = -1;
}

void st_print(const Stack *stack) {
    printf("Stack (top to bottom): ");
    for (int i = stack->top; i >= 0; i--)
        printf("%d ", stack->data[i]);
    printf("\n");
}