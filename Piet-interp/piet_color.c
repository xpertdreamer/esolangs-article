//
// Created by IWOFLEUR on 03.02.2026.
//

#include "piet_common.h"
#include "piet_color.h"

#include <stdio.h>

/*
 *  Complete table of all Piet colors with RGB values and names
 *  Organized by lightness (light, normal, dark) then by hue
 *  Colors are in RGB order (0xFFC0C0 = light red, etc.)
 *  Last two entries are white and black
 */
piet_color_t piet_colors[] = {
    /* Light colors (bright pastel shades) */
    { 0xFFC0C0, "light red",     "lR", 0  },  // pale red
    { 0xFFFFC0, "light yellow",  "lY", 1  },  // pale yellow
    { 0xC0FFC0, "light green",   "lG", 2  },  // pale green
    { 0xC0FFFF, "light cyan",    "lC", 3  },  // pale cyan
    { 0xC0C0FF, "light blue",    "lB", 4  },  // pale blue
    { 0xFFC0FF, "light magenta", "lM", 5  },  // pale magenta
    
    /* Normal colors (full saturation) */
    { 0xFF0000, "red",           "nR", 6  },  // red
    { 0xFFFF00, "yellow",        "nY", 7  },  // yellow
    { 0x00FF00, "green",         "nG", 8  },  // green
    { 0x00FFFF, "cyan",          "nC", 9  },  // cyan
    { 0x0000FF, "blue",          "nB", 10 },  // blue
    { 0xFF00FF, "magenta",       "nM", 11 },  // magenta
    
    /* Dark colors (deeper shades) */
    { 0xC00000, "dark red",      "dR", 12 },  // dark red
    { 0xC0C000, "dark yellow",   "dY", 13 },  // dark yellow
    { 0x00C000, "dark green",    "dG", 14 },  // dark green
    { 0x00C0C0, "dark cyan",     "dC", 15 },  // dark green
    { 0x0000C0, "dark blue",     "dB", 16 },  // dark blue
    { 0xC000C0, "dark magenta",  "dM", 17 },  // dark purple
    
    /* Special colors (white and black) */
    { 0xFFFFFF, "white",         "WW", C_WHITE },  //  white
    { 0x000000, "black",         "BB", C_BLACK }   //  black
};

/*
 *  Convert RGB color value to internal Piet color index
 *  rgb_color 24-bit RGB color value in 0xRRGGBB hexadecimal format
 *  Internal color index (0-19) if color is recognized
 *  -1 if color is not found in Piet color table
 *  Input images may contain colors not in Piet palette -> unknown_color configuration
 */
int piet_get_color_idx(int rgb_color) {
    for (int i = 0; i < N_COLORS; i++) {
        if (rgb_color == piet_colors[i].col)
            return piet_colors[i].c_idx;
    }
    return -1;
}

/*
 *  Convert internal color index to short string representation
 *  color_idx Internal color index (0-19) or C_MARK_INDEX
 *  Pointer to 2-character string constant representing the color
 *  "II" for C_MARK_INDEX (temporary fill marker)
 *  "??" for unknown/invalid color indices
 *  Used for debug output and tracing
 */
const char *piet_cell_to_str(int color_idx) {
    if (color_idx == C_MARK_INDEX)
        return "II";

    for (int i = 0; i < N_COLORS; i++) {
        if (color_idx == piet_colors[i].c_idx)
            return piet_colors[i].s_name;
    }

    eprintf("warning: unknown color index %d in piet_cell_to_str()\n", color_idx);
    return "??";
}

/*
 *  Extract hue component from color index
 *  color_idx Internal color index
 *  Hue value (0-5) for regular colors, or C_WHITE/C_BLACK for specials
 *  Returns -1 for invalid color indices
 */
int piet_get_hue(int color_idx) {
    if (color_idx < C_WHITE)
        return color_idx % N_HUE;

    if (color_idx == C_BLACK || color_idx == C_WHITE)
        return color_idx;

    eprintf("error: unknown color index %d in piet_get_hue()\n", color_idx);
    return -1;
}

/*
 *  Extract lightness component from color index
 *  color_idx Internal color index
 *  Lightness value (0=light, 1=normal, 2=dark) for regular colors
 *  Returns -1 for white, black, or invalid color indices
 */
int piet_get_light(int color_idx) {
    if (color_idx < C_WHITE)
        return color_idx / N_HUE;

    return -1;
}

/*
 *  Check if color index represents white
 *  color_idx Internal color index to check
 *  1 if color is white, 0 otherwise
 */
int piet_is_white(int color_idx) {
    return color_idx == C_WHITE;
}

/*
 *  Check if color index represents black
 *  color_idx Internal color index to check
 *  1 if color is black, 0 otherwise
 */
int piet_is_black(int color_idx) {
    return color_idx == C_BLACK;
}

/*
 *  Check if color index represents a regular Piet color (not white/black)
 *  color_idx Internal color index to check
 *  1 if regular color (0-17), 0 if white, black, or invalid
 */
int piet_is_color(int color_idx) {
    return color_idx >= 0 && color_idx < C_WHITE;
}