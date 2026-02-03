//
// Created by IWOFLEUR on 03.02.2026.
//

#ifndef PIET_COLOR_H
#define PIET_COLOR_H

/*
 *  Piet color system constants
 */

/* Number of distinct hues in Piet color system
 * Red, yellow, green, cyan, blue, magenta
 */
#define N_HUE 6

/*
 * Number of lightness levels for each hue
 * Light, normal (regular), dark
 */
#define N_LIGHT 3

/*
 * Internal index for white color (after all regular colors)
 * White = index 18
 */
#define C_WHITE (N_HUE * N_LIGHT)

/*
 * Internal index for black color (after white)
 * Black = index 19
 */
#define C_BLACK (C_WHITE + 1)

/*
 * Total number of colors including white and black
 * Total = 20 colors
 */
#define N_COLORS (C_BLACK + 1)

/*
 * Special temporary index used during flood fill operations
 * Marks visited cells during area filling
 */
#define C_MARK_INDEX 9999

/*
 * Color manipulation macros
 * Advance color by hue and lightness steps
 * c Current color index
 * h Hue change (0-5, wraps around at 6)
 * l Lightness change (0-2, wraps around at 3)
 * New color index after applying changes
 * Used to calculate color transitions for Piet commands
 */
#define ADV_COLOR(c, h, l) (((((c) % 6) + ((h) % 6) + (6 * ((((c) / 6) + (l)) % 3)))

/*
 * Structure representing a Piet color with all its attributes
 * col RGB color value in 0xRRGGBB hexadecimal format
 * l_name Long descriptive name (e.g., "light red")
 * s_name Short 2-character name for display (e.g., "lR")
 * c_idx Internal color index (0-19) for program use
 */
typedef struct {
    int col;
    char *l_name;
    char *s_name;
    int c_idx;
} piet_color_t;

/*
 *  Global array containing all Piet color definitions
 *  Defined in piet_color.c, declared here for external use
 */
extern piet_color_t piet_colors[];

// Public functions prototypes

/*
 *  Extract hue component from color index
 *  color_idx Internal color index
 *  Hue value (0-5) for regular colors, or C_WHITE/C_BLACK for specials
 *  Returns -1 for invalid color indices
 */
int piet_get_hue(int color_idx);

/*
 *  Extract lightness component from color index
 *  color_idx Internal color index
 *  Lightness value (0=light, 1=normal, 2=dark) for regular colors
 *  Returns -1 for white, black, or invalid color indices
 */
int piet_get_light(int color_idx);

/*
 *  Check if color index represents white
 *  color_idx Internal color index to check
 *  1 if color is white, 0 otherwise
 */
int piet_is_white(int color_idx);

/*
 *  Check if color index represents black
 *  color_idx Internal color index to check
 *  1 if color is black, 0 otherwise
 */
int piet_is_black(int color_idx);


/*
 *  Check if color index represents a regular Piet color (not white/black)
 *  color_idx Internal color index to check
 *  1 if regular color (0-17), 0 if white, black, or invalid
 */
int piet_is_color(int color_idx);
#endif //PIET_COLOR_H
