/*
 * drawchar.h
 * libansilove 1.1.3
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2019 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#ifndef DRAWCHAR_H
#define DRAWCHAR_H

#include <gd.h>
#include <sys/types.h>

void drawchar(gdImagePtr im, const uint8_t *font_data, uint32_t bits,
    uint32_t height, uint32_t column, uint32_t row,
    uint32_t background, uint32_t foreground, uint8_t character);

#endif /* DRAWCHAR_H */
