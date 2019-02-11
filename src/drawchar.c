/*
 * drawchar.c
 * libansilove 1.1.1
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2019 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <inttypes.h>
#include <gd.h>

#include "drawchar.h"

void
drawchar(gdImagePtr im, const uint8_t *font_data, uint32_t bits,
    uint32_t height, uint32_t column, uint32_t row,
    uint32_t background, uint32_t foreground, uint8_t character)
{
	uint32_t x, y;

	gdImageFilledRectangle(im, column * bits, row*height, column * bits +
	    bits - 1, row * height + height - 1, background);

	for (y = 0; y < height; y++) {
		for (x = 0; x < bits; x++) {

			if ((font_data[y+character*height] & (0x80 >> x)) != 0) {
				gdImageSetPixel(im, column * bits + x,
				    row*height + y, foreground);

				if (bits == 9 && x == 7 &&
				    character > 191 && character < 224)
					gdImageSetPixel(im, column * bits + 8,
					    row * height + y, foreground);
			}
		}
	}
}
