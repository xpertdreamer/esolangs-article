//
// Created by IWOFLEUR on 01.02.2026.
//

#include "piet_image.h"
#include <png.h>
#include <stdlib.h>

// Global color table for Piet colors
// Format: RGB value, long name, short name, internal index
Color c_colors[] = {
    {0xFFC0C0, "light red", "lR", 0},        // Light red
    {0xFFFFC0, "light yellow", "lY", 1},     // Light yellow
    {0xC0FFC0, "light green", "lG", 2},      // Light green
    {0xC0FFFF, "light cyan", "lC", 3},       // Light cyan
    {0xC0C0FF, "light blue", "lB", 4},       // Light blue
    {0xFFC0FF, "light magenta", "lM", 5},    // Light magenta

    {0xFF0000, "red", "nR", 6},              // Normal red
    {0xFFFF00, "yellow", "nY", 7},           // Normal yellow
    {0x00FF00, "green", "nG", 8},            // Normal green
    {0x00FFFF, "cyan", "nC", 9},             // Normal cyan
    {0x0000FF, "blue", "nB", 10},            // Normal blue
    {0xFF00FF, "magenta", "nM", 11},         // Normal magenta

    {0xC00000, "dark red", "dR", 12},        // Dark red
    {0xC0C000, "dark yellow", "dY", 13},     // Dark yellow
    {0x00C000, "dark green", "dG", 14},      // Dark green
    {0x00C0C0, "dark cyan", "dC", 15},       // Dark cyan
    {0x0000C0, "dark blue", "dB", 16},       // Dark blue
    {0xC000C0, "dark magenta", "dM", 17},    // Dark magenta

    {0xFFFFFF, "white", "WW", C_WHITE},      // White (special color)
    {0x000000, "black", "BB", C_BLACK}       // Black (special color)
};

// Create new image with specified dimensions
Image* create_piet_image(const int width, const int height) {
    Image* image = (Image*)malloc(sizeof(Image));
    if (image == NULL) return NULL;

    image->width = width;
    image->height = height;
    image->cells = (int*)calloc(width * height, sizeof(int));

    if (image->cells == NULL) {
        free(image);
        return NULL;
    }

    for (int i = 0; i < width * height; i++) {
        image->cells[i] = C_BLACK;
    }

    return image;
}

void free_piet_image(Image* image) {
    if (image != NULL) {
        free(image->cells);
        free(image);
    }
}

// Resize image to new dimensions
void resize_piet_image(Image* image, const int width, const int height) {
    // Allocate new cell array with new dimensions
    int* new_cells = (int*)calloc(width * height, sizeof(int));
    if (new_cells == NULL) return;

    for (int i = 0; i < width * height; i++) {
        new_cells[i] = C_BLACK;
    }

    // Calculate copy dimensions (minimum of old and new)
    int copy_width = (image->width < width) ? image->width : width;
    int copy_height = (image->height < height) ? image->height : height;

    // Copy existing cells to new array
    for (int y = 0; y < copy_height; y++) {
        for (int x = 0; x < copy_width; x++) {
            new_cells[y * width + x] = image->cells[y * width + x];
        }
    }

    free(image->cells);
    image->cells = new_cells;
    image->width = width;
    image->height = height;
}

// Set color index at specified coordinates
int get_piet_image(const Image* image, const int x, const int y) {
    if (!valid_piet_coord(image, x, y)) return -1;
    return image->cells[y * image->width + x];
}

// Set color index at specified coordinates
void set_piet_image(Image* image, const int x, const int y, const int value) {
    if (!valid_piet_coord(image, x, y)) return;
    image->cells[y * image->width + x] = value;
}

// Check if coordinates are within image bounds
int valid_piet_coord(const Image* image , const int x, const int y) {
    return (x >= 0 && x < image->width && y >= 0 && y < image->height);
}

// Print image contents for debugging
void dump_piet_image(const Image* image) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int idx = get_piet_image(image, x, y);     // Get color at position
            printf("%3s", cell2str(idx));
        }
        printf("\n");
    }
}

// Extract hue component from color index
int get_hue(int val) {
    if (val < C_WHITE)
        return val % N_HUE;                     // Hue is remainder when divided by number of hues
    if (val == C_BLACK || val == C_WHITE)
        return val;

    return -1;                                  // Invalid color index
}

// Extract lightness component from color index
int get_light(int val) {
    if (val < C_BLACK)
        return val / N_HUE;      // Lightness is quotient when divided by number of hues
    return -1;                   // Invalid for black/white
}

// Convert RGB color value to internal color index
int get_color_idx(int col) {
    for (int i = 0; i < N_COLORS; i++) {
        if (col == c_colors[i].col)
            return c_colors[i].c_idx;
    }

    return -1;      // Color not found in table
}

// Convert color index to string abbreviation
char* cell2str(int idx) {
    if (idx == C_MARK_INDEX)
        return "II";    // Special marker for flood fill

    for (int i = 0; i < N_COLORS; i++) {
        if (idx == c_colors[i].c_idx)
            return c_colors[i].s_name;
    }

    return "??";    // Unknown color index
}

#ifdef HAVE_PNG_H
// Read PNG file into image structure
int read_png(const char* filename, Image* image, int unknown_color) {
    FILE* f = fopen(filename,"rb");
    if (f == NULL) {
        fprintf(stderr, "Cannot open %s\n", filename);
        return -1;
    }

    // Read PNG signature
    unsigned char header[8];
    if (fread(header, 1, 8, f) != 8 || png_sig_cmp(header, 0, 8) != 0) {
        fclose(f);
        return -1;
    }

    // Create PNG read structure
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fclose(f);
        return -1;
    }

    // Create PNG info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(f);
        return -1;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return -1;
    }

    // Initialize PNG I/O
    png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, 8);

    // Read PNG with transformations
    png_read_png(png_ptr, info_pt,
        PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA |
                    PNG_TRANSFORM_EXPAND, NULL);

    // Get image data
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    // Resize image to match PNG dimensions
    resize_piet_image(image, width, height);

    // Process each pixel
    for (int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++) {
            // Extract RGB components
            int r = row[x * 3];
            int g = row[x * 3 + 1];
            int b = row[x * 3 + 2];

            // Combine into RGB value
            int col = (r << 16) | (g << 8) | b;
            int col_idx = get_color_idx(col);

            if (col_idx < 0) {
                if (unknown_color == -1) {
                    fprintf(stderr, "Invalid color found in PNG [index]: %d\n", col_idx);
                    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                    fclose(f);
                    return -1;
                } else col_idx = unknown_color == 0 ? C_BLACK : C_WHITE;
            }

            set_piet_image(image, x, y, col_idx);
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(f);
    return 0;
}
#endif