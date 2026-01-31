//
// Created by IWOFLEUR on 28.01.2026.
//

#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#include "types.h"

PietColor piet_color_from_rgb(uint8_t r, uint8_t g, uint8_t b);
PietCommand piet_command_from_colors(PietColor from, PietColor to);
const char* piet_color_name(PietColor c);
const char* piet_command_name(PietCommand c);

#endif //COLOR_H
