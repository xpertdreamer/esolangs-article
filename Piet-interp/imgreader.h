//
// Created by IWOFLEUR on 28.01.2026.
//

#ifndef IMGREADER_H
#define IMGREARER_H

#include <stdint.h>

#include "piet.h"

PietColor get_piet_color(uint8_t r, uint8_t g, uint8_t b);
bool read_piet_image(PietState* state, const char* filename);
void print_image_info(const PietState* state);

#endif //IMGREADER_H
