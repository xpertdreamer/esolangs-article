//
// Created by IWOFLEUR on 28.01.2026.
//

#ifndef STACK_H
#define STACK_H

#define INITIAL_CAPACITY 64
#define MAX_STACK_SIZE 65536

#include <stdbool.h>

typedef struct {
    int *data;
    int top;
    int capacity;
} Stack;

Stack* st_init(void);
void st_free(Stack *s);
bool st_clear(Stack *s);
int st_size(const Stack *s);
bool st_is_empty(const Stack *s);
bool st_is_full(const Stack *s);
bool st_push(Stack *s, int value);
int st_pop(Stack *s);
int st_peek(const Stack *s);
bool st_roll(Stack *s, int depth, int rolls);
bool st_duplicate(Stack *s);
bool st_swap(Stack *s);

#endif //STACK_H
