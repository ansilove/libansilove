//
//  ansilove.h
//  AnsiLove/C
//
//  Copyright (C) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include <gd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef drawchar_h
#define drawchar_h

// prototypes
void drawchar(gdImagePtr im, const unsigned char *font_data, int32_t bits,
              int32_t height, int32_t column, int32_t row,
              int32_t background, int32_t foreground, unsigned char character);

#endif
