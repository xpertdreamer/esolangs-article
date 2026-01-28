//
// Created by IWOFLEUR on 28.01.2026.
//

#ifndef PIET_H
#define PIET_H
#include <stdbool.h>

#define MAX_STACK_SIZE 1024

// Piet color definitions (standard 20 colors + black/white)
typedef enum {
    COLOR_LIGHT_RED = 0,
    COLOR_LIGHT_YELLOW,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_MAGENTA,
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_DARK_RED,
    COLOR_DARK_YELLOW,
    COLOR_DARK_GREEN,
    COLOR_DARK_CYAN,
    COLOR_DARK_BLUE,
    COLOR_DARK_MAGENTA,
    COLOR_WHITE,
    COLOR_BLACK,
    COLOR_UNKNOWN
} PietColor;

// Direction Pointer also known as DP
typedef enum {
    DP_RIGHT = 0,
    DP_LEFT,
    DP_DOWN,
    DP_UP,
} Direction;

// Codel Chooser (CC)
typedef enum {
    CC_LEFT = 0,
    CC_RIGHT,
} CodelChooser;

// Operation codes
typedef enum {
    OP_NONE = 0,
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_NOT,
    OP_GREATER,
    OP_POINTER,
    OP_SWITCH,
    OP_DUPLICATE,
    OP_ROLL,
    OP_IN_NUMBER,
    OP_IN_CHAR,
    OP_OUT_NUMBER,
    OP_OUT_CHAR,
} Operation;

// Interpreter state
typedef struct {
    int width;
    int height;
    PietColor **image;
    int x;                  // This two integers represents
    int y;                  // a current position
    Direction dp;
    CodelChooser cc;
    bool running;
} PietState;

typedef struct Stack stack;

// Some API there
PietState* piet_create(int width, int height);
void piet_destroy(PietState* state);
bool piet_load_image(PietState* state, const char* filename);
void piet_run(PietState* state);

#endif //PIET_H
