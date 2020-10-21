/*
 * binary.c
 * libansilove 1.2.6
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
#include "config.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#define STATE_CHARACTER 0
#define STATE_ATTRIBUTE 1

int
ansilove_binary(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	uint8_t character, attribute, *cursor, state = STATE_CHARACTER;
	int32_t column = 0, row = 0;
	uint32_t background, foreground;
	uint32_t width, height;
	uint32_t colors[16];
	size_t loop = 0;
	struct fontStruct fontData;

	/* libgd image pointers */
	gdImagePtr canvas;

	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (!ctx->length) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	/* font selection */
	memset(&fontData, 0, sizeof(struct fontStruct));
	select_font(&fontData, options->font);

	/* Default to 160 columns if columns option wasn't set */
	options->columns = options->columns ? options->columns : 160;

	if (options->columns < 1 || options->columns > 4096) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	width = options->columns * options->bits;
	height = ctx->length / 2 / options->columns * fontData.height;

	if (!width || !height) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	/* allocate buffer image memory */
	canvas = gdImageCreate(width, height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		return -1;
	}

	/* allocate color palette */
	for (size_t i = 0; i < 16; i++)
		colors[i] = gdImageColorAllocate(canvas, vga_palette_red[i],
		    vga_palette_green[i], vga_palette_blue[i]);

	/* process binary */
	while (loop < ctx->length) {
		cursor = &ctx->buffer[loop];

		if (column == options->columns) {
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

			if (background > 8 && !options->icecolors)
				background -= 8;

			drawchar(canvas, fontData.font_data, options->bits,
			    fontData.height, column, row, colors[background],
			    colors[foreground], character);

			column++;

			state = STATE_CHARACTER;
			break;
		}

		loop++;
	}

	/* create output image */
	return output(ctx, options, canvas);
}
