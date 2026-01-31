#ifndef TYPES_H
#define TYPES_H

typedef enum {
  DP_RIGHT = 0,
  DP_DOWN,
  DP_LEFT,
  DP_UP
} PietDirection;

typedef enum {
  CC_LEFT = 0,
  CC_RIGHT
} PietCodelChooser;

typedef enum {
  P_WHITE = 0,
  P_BLACK,
  P_RED,
  P_LIGHT_RED,
  P_DARK_RED,
  P_YELLOW,
  P_LIGHT_YELLOW,
  P_DARK_YELLOW,
  P_GREEN,
  P_LIGHT_GREEN,
  P_DARK_GREEN,
  P_CYAN,
  P_LIGHT_CYAN,
  P_DARK_CYAN,
  P_BLUE,
  P_LIGHT_BLUE,
  P_DARK_BLUE,
  P_MAGENTA,
  P_LIGHT_MAGENTA,
  P_DARK_MAGENTA,
  P_COLOR_NON
} PietColor;

typedef enum {
  P_CMD_NONE = -1,
  P_CMD_PUSH,
  P_CMD_POP,
  P_CMD_ADD,
  P_CMD_SUB,
  P_CMD_MUL,
  P_CMD_DIV,
  P_CMD_MOD,
  P_CMD_NOT,
  P_CMD_GREATER,
  P_CMD_POINTER,
  P_CMD_SWITCH,
  P_CMD_DUPL,
  P_CMD_ROLL,
  P_CMD_IN_NUM,
  P_CMD_IN_CHAR,
  P_CMD_OUT_NUM,
  P_CMD_OUT_CHAR
} PietCommand;
