//
// Created by IWOFLEUR on 03.02.2026.
//

#include "piet_common.h"
#include "piet_color.h"
#include "piet_cell.h"
#include "piet_io.h"
#include <png.h>
#include <string.h>
#include <errno.h>

/*
 *  Read Piet program from PNG file
 *  filename Path to PNG file (or "-" for stdin, though not recommended)
 *  Return 0 on success, -1 on error
 *  Uses libpng to read PNG image, converts to Piet color indices
 *  Supports 24-bit RGB PNGs, handles alpha transparency stripping
 */
int piet_read_png(const char* filename) {
    /*
     * For docs:
     * header PNG file signature (8 bytes)
     * png_ptr PNG read structure pointer
     * info_ptr PNG info structure pointer
     * row_pointers Array of row pointers for image data
     * f File handle for reading
     * color_depth Bit depth of PNG (usually 8)
     * row, col Loop counters for reading pixels
     */
    png_byte header[8];
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep *row_pointers = NULL;
    FILE* f = NULL;
    int color_depth;
    int row, col;

    /*
     * Check for stdin special case (not recommended for PNG)
     * Then open file in binary mode (PNG is binary format)
     * Read and verify PNG signature (first 8 bytes)
     * Check if file has valid PNG signature
     */
    if (strcmp(filename, "-") == 0) {
        vprintf("info: PNG reading from stdin not possible - special case\n");
        return -1;
    }

    f = fopen(filename, "rb");
    if (f == NULL) {
        eprintf("error: cannot open PNG file '%s' : %s\n", filename, strerror(errno));
        return -1;
    }
    if (fread(header, 1, 8, f) != 8) {
        eprintf("error: cannot read PNG signature from '%s'\n", filename);
        fclose(f);
        return -1;
    }
    if (png_sig_cmp(header, 0, 8) != 0) {
        eprintf("error: file '%s' is not a valid PNG image\n", filename);
        fclose(f);
        return -1;
    }

    // Initialize PNG reading structures and setup error handling (required for libpng)

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        eprintf("error: cannot create PNG reading struct\n");
        fclose(f);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        eprintf("error: cannot create PNG info struct\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(f);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        eprintf("error: PNG reading failed from file '%s'\n", filename);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return -1;
    }

    /*
     * Configure PNG reading and read image data:
     *      Set up PNG I/O to use our file handle
     *      Tell libpng we already read the signature
     *      Configure PNG transformations:
     *          - Strip 16-bit samples to 8-bit (Piet uses 8-bit colors)
     *          - Strip alpha channel (Piet doesn't use transparency)
     *          - Expand palettes to RGB (handle indexed color PNGs)
     *      Get image dimensions and format info
     *      Get pointer to row data array
     */
    png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, 8);
    png_read_png(png_ptr, info_ptr,
                 PNG_TRANSFORM_STRIP_16 |   // 16-bit → 8-bit
                 PNG_TRANSFORM_STRIP_ALPHA |    // Remove alpha
                 PNG_TRANSFORM_EXPAND,  // Palette → RGB
                 NULL); // No extras

    width = (int)png_get_image_width(png_ptr, info_ptr);
    height = (int)png_get_image_height(png_ptr, info_ptr);
    color_depth = 1 << png_get_bit_depth(png_ptr, info_ptr);    // Usually 256
    row_pointers = png_get_rows(png_ptr, info_ptr);
    vprintf("info: PNG image: %dx%d pixels, %d color levels\n", width, height, color_depth);

    /*
     * Allocate cell grid and convert pixels.
     * Then cleanup and return
     */
    piet_alloc_cells(width, height);

    for (row = 0; row < height; row++) {
        png_byte* pixel_row = row_pointers[row];
        for (col = 0; col < width; col++) {
            png_byte* pixel = &pixel_row[col * 3];
            int red = pixel[0];
            int green = pixel[1];
            int blue = pixel[2];

            int rgb_color = (red << 16) | (green << 8) | blue;
            int color_index = piet_get_color_idx(rgb_color);

            if (color_index < 0) {
                vprintf("info: unknown color 0x%06x at pixel (%d,%d)\n", rgb_color, col, row);
                // Apply unknown color policy
                if (unknown_color == -1) {
                    // Error policy: fail on unknown colors
                    eprintf("error: unknown color 0x%06x in PNG at (%d,%d)\n",
                            rgb_color, col, row);
                    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                    fclose(f);
                    return -1;
                } else color_index = unknown_color == 0 ? C_BLACK : C_WHITE;
            }

            cells[row * width + col] = color_index;
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(f);
    vprintf("info: successfully loaded PNG file '%s'\n", filename);
    return 0;
}