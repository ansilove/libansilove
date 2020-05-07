/*
 * tundra.c
 * libansilove 1.2.2
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
#include <string.h>
#include "ansilove.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#define TUNDRA_VERSION 24
#define TUNDRA_STRING "TUNDRA24"

#define TUNDRA_HEADER_LENGTH 9 /* 8 + 1 */

#define TUNDRA_POSITION 1
#define TUNDRA_COLOR_BACKGROUND 2
#define TUNDRA_COLOR_FOREGROUND 4
#define TUNDRA_COLOR_BOTH 6

int
ansilove_tundra(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (ctx->length < TUNDRA_HEADER_LENGTH) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	struct fontStruct fontData;
	char tundra_version;

	options->columns = options->columns ? options->columns : 80;
	int16_t columns = options->columns;

	/* font selection */
	memset(&fontData, 0, sizeof(struct fontStruct));
	select_font(&fontData, options->font);

	/* libgd image pointers */
	gdImagePtr canvas;

	/* extract tundra header */
	tundra_version = ctx->buffer[0];

	if (tundra_version != TUNDRA_VERSION ||
	    strncmp((const char *)ctx->buffer + 1, TUNDRA_STRING, 8)) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	/* read tundra file a first time to find the image size */
	uint32_t cursor, character, background = 0, foreground = 0;
	uint32_t loop = TUNDRA_HEADER_LENGTH;
	int32_t column = 0, row = 1;

	while (loop < ctx->length) {
		if (column == columns) {
			column = 0;
			row++;
		}

		cursor = ctx->buffer[loop];

		switch (cursor) {
		case TUNDRA_POSITION:
			row = (ctx->buffer[loop + 1] << 24) +
			    (ctx->buffer[loop + 2] << 16) +
			    (ctx->buffer[loop + 3] << 8) +
			    ctx->buffer[loop + 4];

			column = (ctx->buffer[loop + 5] << 24) +
			    (ctx->buffer[loop + 6] << 16) +
			    (ctx->buffer[loop + 7] << 8) +
			    ctx->buffer[loop + 8];

			loop += 8;
			break;

		case TUNDRA_COLOR_BACKGROUND:
			loop += 5;
			column++;
			break;

		case TUNDRA_COLOR_FOREGROUND:
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


	/* allocate buffer image memory */
	canvas = gdImageCreateTrueColor(columns * options->bits,
	    row * fontData.height);

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
			break;

		case TUNDRA_COLOR_BACKGROUND:
			foreground =
			    (ctx->buffer[loop + 3] << 16) +
			    (ctx->buffer[loop + 4] << 8) +
			    ctx->buffer[loop + 5];

			character = ctx->buffer[loop+1];

			loop += 5;
			break;

		case TUNDRA_COLOR_FOREGROUND:
			background = (ctx->buffer[loop + 3] << 16) +
			    (ctx->buffer[loop + 4] << 8) +
			    ctx->buffer[loop + 5];

			character = ctx->buffer[loop + 1];

			loop += 5;
			break;

		case TUNDRA_COLOR_BOTH:
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
}
