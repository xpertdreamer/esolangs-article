//
// Created by IWOFLEUR on 28.01.2026.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "stack.h"
#include "log.h"

// Internal helper macros
#define CHECK_NULL(s) \
    do { \
        if ((s) == NULL) { \
            PIET_ERROR("Stack is NULL\n"); \
            return false; \
        } \
    } while(0)

#define CHECK_NULL_INT(s) \
    do { \
        if ((s) == NULL) { \
            PIET_ERROR("Stack is NULL\n"); \
            return -1; \
        } \
    } while(0)

#define CHECK_EMPTY(s) \
    do { \
        if (st_is_empty(s)) { \
            PIET_ERROR("Stack is empty\n"); \
            return false; \
        } \
    } while(0)

#define CHECK_EMPTY_INT(s) \
    do { \
        if (st_is_empty(s)) { \
            PIET_ERROR("Stack is empty\n"); \
            return -1; \
        } \
    } while(0)

Stack* st_init(void) {
    Stack* s = malloc(sizeof(Stack));
    if (s == NULL) return NULL;

    s->capacity = INITIAL_CAPACITY;
    s->top = -1;
    s->data = malloc(s->capacity * sizeof(int));
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    return s;
}

void st_free(Stack *s) {
    if (s == NULL) return;

    free(s->data);
    s->data = NULL;
    free(s);
}

bool st_clear(Stack *s) {
    CHECK_NULL(s);
    s->top = -1;
    return true;
}

int st_size(const Stack *s) {
    if (s == NULL) {
        PIET_ERROR("Stack is NULL\n");
        return -1;
    }
    return s->top + 1;
}

bool st_is_empty(const Stack *s) {
    if (s == NULL) {
        PIET_ERROR("Stack is NULL\n");
        return true;
    }
    return s->top == -1;
}

bool st_is_full(const Stack *s) {
    if (s == NULL) {
        PIET_ERROR("Stack is NULL\n");
        return false;
    }
    return st_size(s) >= MAX_STACK_SIZE;
}

bool st_push(Stack *s, int value) {
    CHECK_NULL(s);

    // Check for stack overflow
    if (st_size(s) >= MAX_STACK_SIZE) {
        PIET_ERROR("Stack overflow (max size: %d)\n", MAX_STACK_SIZE);
        return false;
    }

    // Resize if needed
    if (st_size(s) >= s->capacity) {
        // Check for integer overflow
        if (s->capacity > INT_MAX / 2) {
            PIET_ERROR("Capacity overflow\n");
            return false;
        }

        size_t new_capacity = s->capacity * 2;
        // Cap at MAX_STACK_SIZE
        if (new_capacity > MAX_STACK_SIZE) {
            new_capacity = MAX_STACK_SIZE;
        }

        int* new_data = realloc(s->data, new_capacity * sizeof(int));
        if (new_data == NULL) {
            PIET_ERROR("Memory allocation failed during stack resize\n");
            return false;
        }

        s->data = new_data;
        s->capacity = new_capacity;
    }

    s->top++;
    s->data[s->top] = value;
    return true;
}

int st_pop(Stack *s) {
    CHECK_NULL_INT(s);
    CHECK_EMPTY_INT(s);

    return s->data[s->top--];
}

int st_peek(const Stack *s) {
    CHECK_NULL_INT(s);
    CHECK_EMPTY_INT(s);

    return s->data[s->top];
}

bool st_duplicate(Stack *s) {
    CHECK_NULL(s);
    CHECK_EMPTY(s);

    int top_value = st_peek(s);
    return st_push(s, top_value);
}

bool st_swap(Stack *s) {
    CHECK_NULL(s);

    if (st_size(s) < 2) {
        PIET_ERROR("Cannot swap: stack needs at least 2 elements\n");
        return false;
    }

    int a = st_pop(s);
    int b = st_pop(s);

    bool success = st_push(s, a);
    if (!success) return false;

    return st_push(s, b);
}

bool st_roll(Stack *s, int depth, int rolls) {
    // Leave it alone for now
}