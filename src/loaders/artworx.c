/*
 * artworx.c
 * libansilove 1.2.3
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <gd.h>
#include <stddef.h>
#include <stdint.h>
#include "ansilove.h"
#include "drawchar.h"
#include "output.h"

#define ADF_HEADER_LENGTH 4289 /* 192 + 4096 + 1 */

int
ansilove_artworx(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (ctx->length < ADF_HEADER_LENGTH) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	/* libgd image pointers */
	gdImagePtr canvas;

	/* create ADF instance */
	canvas = gdImageCreate(640, (((ctx->length - ADF_HEADER_LENGTH) / 2) / 80) * 16);
	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		return -1;
	}

	/* ADF color palette array */
	uint32_t adf_colors[16] = { 0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63 };

	size_t index, loop;

	/* process ADF palette */
	for (loop = 0; loop < 16; loop++) {
		index = (adf_colors[loop] * 3) + 1;
		gdImageColorAllocate(canvas, (ctx->buffer[index] << 2 | ctx->buffer[index] >> 4),
		    (ctx->buffer[index + 1] << 2 | ctx->buffer[index + 1] >> 4),
		    (ctx->buffer[index + 2] << 2 | ctx->buffer[index + 2] >> 4));
	}

	/* process ADF */
	uint32_t column = 0, row = 0;
	uint32_t character, attribute, foreground, background;
	loop = ADF_HEADER_LENGTH;

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

	/* create output file */
	return output(ctx, options, canvas);
}
