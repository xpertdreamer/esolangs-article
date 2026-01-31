//
// Created by IWOFLEUR on 31.01.2026.
//

#include "types.h"
#include "color.h"

// Official colors from specs
static const struct {
    int r, g, b;
    PietColor color;
    const char *name;
} piet_colors[] = {
    {255, 255, 255, P_WHITE, "white"},
    {0, 0, 0, P_BLACK, "black"},
    {255, 192, 192, P_LIGHT_RED, "light red"},
    {255, 0, 0, P_RED, "red"},
    {192, 0, 0, P_DARK_RED, "dark red"},
    {255, 255, 192, P_LIGHT_YELLOW, "light yellow"},
    {255, 255, 0, P_YELLOW, "yellow"},
    {192, 192, 0, P_DARK_YELLOW, "dark yellow"},
    {192, 255, 192, P_LIGHT_GREEN, "light green"},
    {0, 255, 0, P_GREEN, "green"},
    {0, 192, 0, P_DARK_GREEN, "dark green"},
    {192, 255, 255, P_LIGHT_CYAN, "light cyan"},
    {0, 255, 255, P_CYAN, "cyan"},
    {0, 192, 192, P_DARK_CYAN, "dark cyan"},
    {192, 192, 255, P_LIGHT_BLUE, "light blue"},
    {0, 0, 255, P_BLUE, "blue"},
    {0, 0, 192, P_DARK_BLUE, "dark blue"},
    {255, 192, 255, P_LIGHT_MAGENTA, "light magenta"},
    {255, 0, 255, P_MAGENTA, "magenta"},
    {192, 0, 192, P_DARK_MAGENTA, "dark magenta"},
};

PietColor piet_color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    int best_i = 0;
    int best_dist = 255 * 255 * 3 + 1;

    for (int i = 0; i < sizeof(piet_colors) / sizeof(piet_colors[0]); i++) {
        int dr = r - piet_colors[i].r;
        int dg = g - piet_colors[i].g;
        int db = b - piet_colors[i].b;
        int dist = dr * dr + dg * dg + db * db;

        if (dist < best_dist) {
            best_dist = dist;
            best_i = i;
        }
    }

    return piet_colors[best_i].color;
}

PietCommand piet_command_from_colors(PietColor from, PietColor to) {
    if (from == P_WHITE || from == P_BLACK || to == P_WHITE || to == P_BLACK)
        return P_CMD_NONE;

    int hue_from = (from - 2) / 3;
    int lightness_from = (from - 2) % 3;
    int hue_to = (to - 2) / 3;
    int lightness_to = (to - 2) % 3;

    int hue_diff = (hue_from - hue_to + 6) % 6;
    int lightness_diff = (lightness_from - lightness_to + 3) % 3;

    static const PietCommand cmd_table[6][3] = {
        {P_CMD_PUSH, P_CMD_POP, P_CMD_ADD},
        {P_CMD_SUB, P_CMD_MUL, P_CMD_DIV},
        {P_CMD_MOD, P_CMD_NOT, P_CMD_GREATER},
        {P_CMD_POINTER, P_CMD_SWITCH, P_CMD_DUPL},
        {P_CMD_ROLL, P_CMD_IN_NUM, P_CMD_IN_CHAR},
        {P_CMD_OUT_NUM, P_CMD_OUT_CHAR, P_CMD_NONE}
    };

    return cmd_table[hue_diff][lightness_diff];
}

const char* piet_color_name(PietColor c) {
    if (c >= 0 && c < P_COLOR_COUNT) {
        return piet_colors[c].name;
    }

    return "unknown";
}

const char* piet_command_name(PietCommand c) {
    static const char *names[] = {
        "push", "pop", "add", "subtract", "multiply", "divide", "mod",
        "not", "greater", "pointer", "switch", "duplicate", "roll",
        "in_num", "in_char", "out_num", "out_char"
    };

    if (c >= 0 && c < P_CMD_COUNT)
        return names[c];

    return "none";
}