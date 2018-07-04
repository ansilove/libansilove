//
//  artworx.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "../ansilove.h"

int ansilove_artworx(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	// libgd image pointers
	gdImagePtr canvas;

	// create ADF instance
	canvas = gdImageCreate(640, (((ctx->length - 192 - 4096 -1) / 2) / 80) * 16);

	// error output
	if (!canvas) {
		ctx->error = GD_ERROR;
		return -1;
	}

	// ADF color palette array
	uint32_t adf_colors[16] = { 0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63 };

	uint32_t loop;
	uint32_t index;

	// process ADF palette
	for (loop = 0; loop < 16; loop++) {
		index = (adf_colors[loop] * 3) + 1;
		gdImageColorAllocate(canvas, (ctx->buffer[index] << 2 | ctx->buffer[index] >> 4),
		    (ctx->buffer[index + 1] << 2 | ctx->buffer[index + 1] >> 4),
		    (ctx->buffer[index + 2] << 2 | ctx->buffer[index + 2] >> 4));
	}

	gdImageColorAllocate(canvas, 0, 0, 0);

	// process ADF
	uint32_t column = 0, row = 0;
	uint32_t character, attribute, foreground, background;
	loop = 192 + 4096 + 1;

	while (loop < ctx->length) {
		if (column == 80) {
			column = 0;
			row++;
		}

		character = ctx->buffer[loop];
		attribute = ctx->buffer[loop+1];

		background = (attribute & 240) >> 4;
		foreground = attribute & 15;

		drawchar(canvas, ctx->buffer+193, 8, 16, column, row, background, foreground, character);

		column++;
		loop += 2;
	}

	// create output file
	output(canvas, options->fileName, options->retina, options->retinaScaleFactor);

	return 0;
}
