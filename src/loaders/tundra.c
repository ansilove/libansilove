/*
 * tundra.c
 * libansilove 1.4.1
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2025 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause license.
 * See LICENSE file for details.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <gd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "ansilove.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#define TUNDRA_VERSION 24
#define TUNDRA_STRING "TUNDRA24"

#define TUNDRA_HEADER_LENGTH 9 /* 8 + 1 */

#define TUNDRA_POSITION 1
#define TUNDRA_COLOR_FOREGROUND 2
#define TUNDRA_COLOR_BACKGROUND 4
#define TUNDRA_COLOR_BOTH 6

int
ansilove_tundra(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	char tundra_version;
	int32_t column = 0, row = 1;
	uint32_t cursor, character, background = 0, foreground = 0;
	uint32_t width, height;
	size_t loop = TUNDRA_HEADER_LENGTH;
	struct fontStruct fontData;

	/* libgd image pointers */
	gdImagePtr canvas;

	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (ctx->length < TUNDRA_HEADER_LENGTH)
		goto error;

	if (options->bits != 8 && options->bits !=9) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	options->columns = options->columns ? options->columns : 80;
	int16_t columns = options->columns;

	if (columns < 1 || columns > 4096) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	/* font selection */
	memset(&fontData, 0, sizeof(struct fontStruct));
	select_font(&fontData, options->font);

	/* extract tundra header */
	tundra_version = ctx->buffer[0];

	if (tundra_version != TUNDRA_VERSION ||
	    strncmp((const char *)ctx->buffer + 1, TUNDRA_STRING, 8))
		goto error;

	/* read tundra file a first time to find the image size */
	while (loop < ctx->length) {
		if (column == columns) {
			column = 0;
			row++;
		}

		cursor = ctx->buffer[loop];

		switch (cursor) {
		case TUNDRA_POSITION:
			if (loop + 8 < ctx->length) {
				row = (ctx->buffer[loop + 1] << 24) +
				    (ctx->buffer[loop + 2] << 16) +
				    (ctx->buffer[loop + 3] << 8) +
				    ctx->buffer[loop + 4];

				column = (ctx->buffer[loop + 5] << 24) +
				    (ctx->buffer[loop + 6] << 16) +
				    (ctx->buffer[loop + 7] << 8) +
				    ctx->buffer[loop + 8];

				loop += 8;
			} else {
				goto error;
			}

			break;

		case TUNDRA_COLOR_FOREGROUND:
		case TUNDRA_COLOR_BACKGROUND:
			loop += 5;
			column++;
			break;

		case TUNDRA_COLOR_BOTH:
			loop += 9;
			column++;
			break;

		default:
			column++;
		}

		loop++;
	}

	width = columns * options->bits;
	height = row * fontData.height;

	if (!width || !height)
		goto error;

	/* allocate buffer image memory */
	canvas = gdImageCreateTrueColor(width, height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		return -1;
	}

	/* process tundra */
	column = 0;
	row = 0;

	loop = TUNDRA_HEADER_LENGTH;

	while (loop < ctx->length) {
		if (column == columns) {
			column = 0;
			row++;
		}

		cursor = character = ctx->buffer[loop];

		switch (cursor) {
		case TUNDRA_POSITION:
			if (loop + 8 < ctx->length) {
				row = (ctx->buffer[loop + 1] << 24) +
				    (ctx->buffer[loop + 2] << 16) +
				    (ctx->buffer[loop + 3] << 8) +
				    ctx->buffer[loop + 4];

				column =
				    (ctx->buffer[loop + 5] << 24) +
				    (ctx->buffer[loop + 6] << 16) +
				    (ctx->buffer[loop + 7] << 8) +
				    ctx->buffer[loop + 8];

				loop += 8;
			} else {
				goto error;
			}

			break;

		case TUNDRA_COLOR_FOREGROUND:
			if (loop + 5 < ctx->length) {
				foreground =
				    (ctx->buffer[loop + 3] << 16) +
				    (ctx->buffer[loop + 4] << 8) +
				    ctx->buffer[loop + 5];

				character = ctx->buffer[loop+1];

				loop += 5;
			} else {
				goto error;
			}

			break;

		case TUNDRA_COLOR_BACKGROUND:
			if (loop + 5 < ctx->length) {
				background = (ctx->buffer[loop + 3] << 16) +
				    (ctx->buffer[loop + 4] << 8) +
				    ctx->buffer[loop + 5];

				character = ctx->buffer[loop + 1];

				loop += 5;
			} else {
				goto error;
			}

			break;

		case TUNDRA_COLOR_BOTH:
			if (loop + 9 < ctx->length) {
				foreground =
				    (ctx->buffer[loop + 3] << 16) +
				    (ctx->buffer[loop + 4] << 8) +
				    ctx->buffer[loop + 5];

				background =
				    (ctx->buffer[loop + 7] << 16) +
				    (ctx->buffer[loop + 8] << 8) +
				    ctx->buffer[loop + 9];

				character = ctx->buffer[loop + 1];

				loop += 9;
			} else {
				goto error;
			}

			break;
		}

		if (character != TUNDRA_POSITION &&
		    character != TUNDRA_COLOR_BACKGROUND &&
		    character != TUNDRA_COLOR_FOREGROUND &&
		    character != TUNDRA_COLOR_BOTH) {
			drawchar(canvas, fontData.font_data, options->bits,
			    fontData.height, column, row, background,
			    foreground, character);

			column++;
		}

		loop++;
	}

	/* create output image */
	return output(ctx, options, canvas);

error:
	ctx->error = ANSILOVE_FORMAT_ERROR;
	return -1;
}
