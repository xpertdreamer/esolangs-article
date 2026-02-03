//
// Created by IWOFLEUR on 03.02.2026.
//

#include "piet_common.h"
#include "piet_color.h"

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

