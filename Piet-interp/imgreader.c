//
// Created by IWOFLEUR on 28.01.2026.
//

#include "imgreader.h"
#include <png.h>

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Piet standard colors (RGB values)
typedef struct {
    uint8_t r, g, b;
} RGBColor;

static const RGBColor palette[20] = {
    // Light colors
    {0xFF, 0xC0, 0xC0}, // LIGHT_RED
    {0xFF, 0xFF, 0xC0}, // LIGHT_YELLOW
    {0xC0, 0xFF, 0xC0}, // LIGHT_GREEN
    {0xC0, 0xFF, 0xFF}, // LIGHT_CYAN
    {0xC0, 0xC0, 0xFF}, // LIGHT_BLUE
    {0xFF, 0xC0, 0xFF}, // LIGHT_MAGENTA
    // Normal colors
    {0xFF, 0x00, 0x00}, // RED
    {0xFF, 0xFF, 0x00}, // YELLOW
    {0x00, 0xFF, 0x00}, // GREEN
    {0x00, 0xFF, 0xFF}, // CYAN
    {0x00, 0x00, 0xFF}, // BLUE
    {0xFF, 0x00, 0xFF}, // MAGENTA
    // Dark colors
    {0xC0, 0x00, 0x00}, // DARK_RED
    {0xC0, 0xC0, 0x00}, // DARK_YELLOW
    {0x00, 0xC0, 0x00}, // DARK_GREEN
    {0x00, 0xC0, 0xC0}, // DARK_CYAN
    {0x00, 0x00, 0xC0}, // DARK_BLUE
    {0xC0, 0x00, 0xC0}, // DARK_MAGENTA
    // White and Black
    {0xFF, 0xFF, 0xFF}, // WHITE
    {0x00, 0x00, 0x00}  // BLACK
};

static int color_distance(const uint8_t r, const uint8_t g, const uint8_t b,
                            const uint8_t r_n, const uint8_t g_n, const uint8_t b_n) {
    const int dr = r - r_n;
    const int dg = g - g_n;
    const int db = b - b_n;
    return dr * dr + dg * dg + db * db;
}

PietColor get_color(uint8_t r, uint8_t g, uint8_t b) {
    int min_dist = INT_MAX;
    int best_match = COLOR_UNKNOWN;

    for (int i = 0; i < 20; i++) {
        int dist = color_distance(r, g, b, palette[i].r, palette[i].g, palette[i].b);

        if (dist < min_dist) {
            min_dist = dist;
            best_match = i;
        }
    }

    // If not close enough treat as white
    if (min_dist > 10000)
        return COLOR_WHITE;

    return (PietColor)best_match;
}

bool read_piet_image(PietState* state, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return false;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png) {
        fclose(f);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(f);
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(f);
        return false;
    }

    png_init_io(png, f);
    png_read_info(png, info);

    state->width = (int)png_get_image_width(png, info);
    state->height = (int)png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format
    if (bit_depth == 16)
        png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    // Allocate memory for image data
    state->image = (PietColor**)malloc(sizeof(PietColor*) * state->height);
    for (int i = 0; i < state->height; i++) {
        state->image[i] = (PietColor*)malloc(sizeof(PietColor) * state->width);
    }

    // Read image data
    png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * state->height);
    for (int i = 0; i < state->height; i++) {
        rows[i] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, rows);

    // Convert to Piet colors
    for (int y = 0; y < state->height; y++) {
        png_bytep row = rows[y];
        for (int x = 0; x < state->width; x++) {
            png_bytep px = &row[x * 4];
            state->image[y][x] = get_color(px[0], px[1], px[2]);
        }
        free(rows[y]);
    }
    free(rows);

    fclose(f);
    png_destroy_read_struct(&png, &info, NULL);

    return true;
}

void print_image_info(const PietState* state) {
    printf("Image info: %dx%d\n", state->width, state->height);

    int colors_count[21] = {0};
    for (int y = 0; y < state->height; y++) {
        for (int x = 0; x < state->width; x++) {
            colors_count[state->image[y][x]]++;
        }
    }

    const char* color_names[] = {
        "Light Red", "Light Yellow", "Light Green", "Light Cyan", "Light Blue", "Light Magenta",
        "Red", "Yellow", "Green", "Cyan", "Blue", "Magenta",
        "Dark Red", "Dark Yellow", "Dark Green", "Dark Cyan", "Dark Blue", "Dark Magenta",
        "White", "Black", "Unknown"
    };

    for (int i = 0; i < 21; i++) {
        if (colors_count[i] > 0) {
            printf("%s: %d pixels\n", color_names[i], colors_count[i]);
        }
    }
}