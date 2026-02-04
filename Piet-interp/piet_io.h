//
// Created by IWOFLEUR on 03.02.2026.
//

#ifndef PIET_IO_H
#define PIET_IO_H

/*
 *  Read Piet program from PNG file
 *  filename Path to PNG file (or "-" for stdin, though not recommended)
 *  Return 0 on success, -1 on error
 *  Uses libpng to read PNG image, converts to Piet color indices
 *  Supports 24-bit RGB PNGs, handles alpha transparency stripping
 */
int piet_read_png(const char* filename);

/*
 * Maybe later this project would contain PPM file support
 * but for now only PNG
 */

#endif //PIET_IO_H
