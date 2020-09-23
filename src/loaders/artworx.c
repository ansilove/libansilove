/*
 * artworx.c
 * libansilove 1.2.4
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
#include "config.h"
#include "drawchar.h"
#include "output.h"

#define ADF_HEADER_LENGTH 4289 /* 192 + 4096 + 1 */

#define STATE_CHARACTER 0
#define STATE_ATTRIBUTE 1

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

	size_t index, loop;

	/* process ADF palette */
	for (loop = 0; loop < 16; loop++) {
		index = (adf_colors[loop] * 3) + 1;
		gdImageColorAllocate(canvas, (ctx->buffer[index] << 2 | ctx->buffer[index] >> 4),
		    (ctx->buffer[index + 1] << 2 | ctx->buffer[index + 1] >> 4),
		    (ctx->buffer[index + 2] << 2 | ctx->buffer[index + 2] >> 4));
	}

	/* process ADF */
	uint8_t character, attribute, *cursor, state = STATE_CHARACTER;
	uint32_t column = 0, row = 0;
	uint32_t foreground, background;
	loop = ADF_HEADER_LENGTH;

	while (loop < ctx->length) {
		cursor = &ctx->buffer[loop];

		if (column == 80) {
			column = 0;
			row++;
		}

		switch (state) {
		case STATE_CHARACTER:
			character = *cursor;
			state = STATE_ATTRIBUTE;
			break;
		case STATE_ATTRIBUTE:
			attribute = *cursor;

			background = (attribute & 240) >> 4;
			foreground = attribute & 15;

			drawchar(canvas, ctx->buffer+193, 8, 16, column, row, background, foreground, character);

			column++;

			state = STATE_CHARACTER;
			break;
		}

		loop++;
	}

	/* create output file */
	return output(ctx, options, canvas);
}
