//
//  ansilove.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "drawchar.h"
#include <gd.h>

// shared method for drawing characters
void drawchar(gdImagePtr im, const unsigned char *font_data, uint32_t bits,
    uint32_t height, uint32_t column, uint32_t row,
    uint32_t background, uint32_t foreground, unsigned char character) {
	uint32_t x, y;

	gdImageFilledRectangle(im, column * bits, row*height, column * bits +
	    bits - 1, row * height + height - 1, background);

	for (y = 0; y < height; y++) {
		for (x = 0; x < bits; x++) {

			if ((font_data[y+character*height] & (0x80 >> x)) != 0) {
				gdImageSetPixel(im, column * bits + x, row*height + y, foreground);

				if (bits == 9 && x == 7 && character > 191 && character < 224)
					gdImageSetPixel(im, column * bits + 8, row * height + y, foreground);
			}
		}
	}
}
