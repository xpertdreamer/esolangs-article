//
// Created by IWOFLEUR on 31.01.2026.
//

#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include "types.h"

typedef struct {
    PietColor* data;
    int width, height, size;
} PietImage;

PietImage* piet_load_png(const char* filename);
void piet_free_png(PietImage* image);
PietColor piet_get_image(const PietImage* image, int x, int y);
void piet_set_image(PietImage* image, int x, int y, PietColor color);

#endif //PNG_LOADER_H
