//
// Created by IWOFLEUR on 01.02.2026.
//
#pragma once

#ifndef PIET_IMAGE_H
#define PIET_IMAGE_H

#include "piet_types.h"

typedef struct {
    int* cells;     // 2d array of color indices (row-major)
    int width;
    int height;
} Image;

// Basic image operations
Image* create_piet_image(int width, int height);
void free_piet_image(Image* image);
void resize_piet_image(Image* image, int width, int height);
int get_piet_image(const Image* image, int x, int y);
void set_piet_image(const Image* image, int x, int y, int value);
int valid_piet_coord(const Image* image ,int x, int y);
void dump_piet_image(Image* image);

// Color space operations
int get_hue(int val);                 // Extract hue component from color index
int get_light(int val);               // Extract lightness component from color index
int get_color_idx(int col);           // Convert RGB color to internal index
char* cell2str(int idx);              // Convert color index to string representation

// PNG file support (conditional compilation)
#ifdef HAVE_PNG_H
int read_png(const char* filename, Image* image, int unknown_color);
#endif

#endif //PIET_IMAGE_H
