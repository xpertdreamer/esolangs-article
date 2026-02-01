//
// Created by IWOFLEUR on 01.02.2026.
//
#pragma once

#ifndef PIET_TYPES_H
#define PIET_TYPES_H

#define VERSION "v0.0.1"            // Version
#define N_HUE 6                     // Number of hues
#define N_LIGHT 3                   // Number of lightness levels
#define C_WHITE (N_HUE * N_LIGHT)   // White index
#define C_BLACK (C_WHITE + 1)       // Black index
#define N_COLORS (C_BLACK + 1)      // Total number of colors
#define C_MARK_INDEX 9999           // Special index for marking cells during flood fill

// DP
#define P_LEFT 'l'
#define P_RIGHT 'r'
#define P_UP 'u'
#define P_DOWN 'd'

// Direction manipulation macros
#define TOGGLE_CC(cc) ((cc) == 'r' ? 'l' : 'r')  // Toggle codel chooser
#define TURN_DP(dp) ((dp) == 'r' ? 'd' : ((dp) == 'd' ? 'l' : ((dp) == 'l' ? 'u' : 'r')))  // Rotate DP clockwise
#define TURN_DP_INV(dp) ((dp) == 'r' ? 'u' : ((dp) == 'u' ? 'l' : ((dp) == 'l' ? 'd' : 'r')))  // Rotate DP counter-clockwise
#define DP_DX(dp) ((dp) == 'l' ? -1 : ((dp) == 'r' ? 1 : 0))  // Get X delta for direction
#define DP_DY(dp) ((dp) == 'u' ? -1 : ((dp) == 'd' ? 1 : 0))  // Get Y delta for direction

// Stack manipulation macros
#define STACK_PUSH(stack, val) do { \
    if ((stack)->num_stack >= (stack)->max_stack) { \
        stack_resize(stack, (stack)->max_stack * 2); \
    } \
    (stack)->data[(stack)->num_stack++] = (val); \
} while (0)
#define STACK_POP(stack) ((stack)->num_stack > 0 ? (stack)->data[--(stack)->num_stack] : 0)
#define STACK_PEEK(stack) ((stack)->num_stack > 0 ? (stack)->data[(stack)->num_stack - 1] : 0)

// Debug macros
#ifdef DEBUG
    #define DPRINTF(...) if (debug) printf(__VA_ARGS__)             // Debug print only when debug enabled
    #define D2PRINTF(...) if (debug > 1) printf(__VA_ARGS__)        // Verbose debug print
#else
    #define DPRINTF(...)
    #define D2PRINTF(...)
#endif

#define VPRINTF(...) if (verbose) printf(__VA_ARGS__)               // Verbose output
#define TPRINTF(...) if (trace) printf(__VA_ARGS__)                 // Trace output
#define T2PRINTF(...) if (trace > 1) printf(__VA_ARGS__)            // Detailed trace output

// Color structure defenition
typedef struct {
    int col;        // RGB color value (0xRRGGBB format)
    char *l_name;   // Long color name (e.g., "light red")
    char *s_name;   // Short color name (e.g., "lR")
    int c_idx;      // Internal color index
} Color;

// External declaration of color table
extern Color c_colors[];

#endif //PIET_TYPES_H
