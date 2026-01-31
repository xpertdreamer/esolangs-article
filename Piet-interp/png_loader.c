//
// Created by IWOFLEUR on 31.01.2026.
//

#include "png_loader.h"
#include "color.h"
#include "log.h"
#include <png.h>
#include <stdlib.h>

PietImage* piet_load_png(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) PIET_ERROR("Cannot open file: %s\n", filename);

    // Verify png signature
    png_byte header[8];
    if (fread(header, 1, 8, file) != 8) {
        fclose(file);
        PIET_ERROR("Cannot read PNG header from: %s\n", filename);
    }
    if (png_sig_cmp(header, 0, 8)) {
        fclose(file);
        PIET_ERROR("Invalid PNG header from file: %s\n", filename);
    }

    // Initialize PNG structures
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(file);
        PIET_ERROR("Cannot create PNG read struct\n");
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(file);
        PIET_ERROR("Cannot create PNG info struct\n");
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        PIET_ERROR("PNG read error\n");
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, 8);

    // Read file info
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    // Transforms
    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB || PNG_COLOR_TYPE_GRAY || PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    // Allocate image
    PietImage* image = malloc(sizeof(PietImage));
    if (!image) {
        fclose(file);
        PIET_ERROR("Cannot allocate Piet image\n");
    }
    image->width = width;
    image->height = height;
    image->size = width * height;
    image->data = malloc(image->size * sizeof(PietColor));

    // Allocate row pointers
    int rowbytes = png_get_rowbytes(png, info);
    png_bytep* row_pointers = malloc(height * sizeof(png_bytep));
    for (int y = 0; y < height; y++) {
        row_pointers[y] = malloc(rowbytes);
    }

    png_read_image(png, row_pointers);

    int channels = png_get_channels(png, info);
    for (int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++) {
            png_bytep px = &row[x * channels];
            PietColor color = piet_color_from_rgb(px[0], px[1], px[2]);
            piet_set_image(image, x, y, color);
        }
    }

    // Cleanup
    for (int y = 0; y < height; y++) free(row_pointers[y]);
    free(row_pointers);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);

    PIET_INFO("Loaded PNG image: %s (%dx%d)\n", filename, width, height);
    return image;
}

void piet_free_png(PietImage* image) {
    if (image != NULL) {
        free(image->data);
        free(image);
    }
}

PietColor piet_get_image(const PietImage* image, int x, int y) {
    if (image == NULL || x < 0 || y < 0 || x >= image->width || y >= image->height)
        return P_BLACK; // Treat out of bounds as black
    return image->data[y * image->width + x];
}

void piet_set_image(PietImage* image, int x, int y, PietColor color) {
    if (image != NULL && x >= 0 && y >= 0 && x < image->width && y < image->height) {
        image->data[y * image->width + x] = color;
    }
}