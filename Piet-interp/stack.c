//
// Created by IWOFLEUR on 28.01.2026.
//
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "stack.h"

void st_init(Stack* stack) {
    stack->capacity = INITIAL_CAPACITY;
    stack->top = -1;
    stack->data = malloc(stack->capacity * sizeof(int64_t));
}

void st_free(Stack *s) {
    if (s == NULL) return;
    free(s->data);
    // Note: don't free(s) here - piet_free handles that
}

bool st_clear(Stack *s) {
    if (s == NULL) return false;
    s->top = -1;
    return true;
}

int st_size(const Stack *s) {
    if (s == NULL) return -1;
    return s->top + 1;
}

bool st_is_empty(const Stack *s) {
    if (s == NULL) return true;
    return s->top == -1;
}

bool st_is_full(const Stack *s) {
    if (s == NULL) return false;
    return st_size(s) >= MAX_STACK_SIZE;
}

bool st_push(Stack *s, int64_t value) {
    if (s == NULL) return false;

    if (st_size(s) >= MAX_STACK_SIZE) {
        return false;
    }

    if (st_size(s) >= s->capacity) {
        size_t new_capacity = s->capacity * 2;
        if (new_capacity > MAX_STACK_SIZE) {
            new_capacity = MAX_STACK_SIZE;
        }

        int64_t* new_data = realloc(s->data, new_capacity * sizeof(int64_t));
        if (new_data == NULL) return false;

        s->data = new_data;
        s->capacity = new_capacity;
    }

    s->top++;
    s->data[s->top] = value;
    return true;
}

int64_t st_pop(Stack *s) {
    if (s == NULL || st_is_empty(s)) {
        return 0;
    }
    return s->data[s->top--];
}

int64_t st_peek(const Stack *s) {
    if (s == NULL || st_is_empty(s)) {
        return 0;
    }
    return s->data[s->top];
}

bool st_roll(Stack *s, int64_t depth, int64_t rolls) {
    if (s == NULL || depth <= 0 || st_size(s) < depth) {
        return false;
    }

    // Normalize rolls
    rolls = rolls % depth;
    if (rolls < 0) rolls += depth;
    if (rolls == 0) return true;

    // Perform roll
    int64_t *temp = malloc(depth * sizeof(int64_t));
    for (int i = 0; i < depth; i++) {
        temp[i] = s->data[s->top - depth + 1 + i];
    }

    // Rotate
    for (int i = 0; i < depth; i++) {
        int src_idx = (i - rolls + depth) % depth;
        s->data[s->top - depth + 1 + i] = temp[src_idx];
    }

    free(temp);
    return true;
}

bool st_duplicate(Stack *s) {
    if (s == NULL || st_is_empty(s)) return false;
    return st_push(s, s->data[s->top]);
}

bool st_swap(Stack *s) {
    if (s == NULL || st_size(s) < 2) return false;

    int64_t a = s->data[s->top];
    int64_t b = s->data[s->top - 1];
    s->data[s->top] = b;
    s->data[s->top - 1] = a;
    return true;
}
