/*
 * drawchar.h
 * libansilove 1.0.0
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <gd.h>
#include <sys/types.h>

#ifndef DRAWCHAR_H
#define DRAWCHAR_H

// prototypes
void drawchar(gdImagePtr im, const unsigned char *font_data, uint32_t bits,
    uint32_t height, uint32_t column, uint32_t row,
    uint32_t background, uint32_t foreground, unsigned char character);

#endif /* DRAWCHAR_H */
