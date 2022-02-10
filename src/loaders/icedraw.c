/*
 * icedraw.c
 * libansilove 1.2.9
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2022 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause license.
 * See LICENSE file for details.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <gd.h>
#include <stdint.h>
#include <stdlib.h>
#include "ansilove.h"
#include "drawchar.h"
#include "output.h"

#define IDF_HEADER_LENGTH 12
#define IDF_FONT_LENGTH 4096
#define IDF_PALETTE_LENGTH 48

#define STATE_CHARACTER 0
#define STATE_ATTRIBUTE 1
#define STATE_RLE 2

int
ansilove_icedraw(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	size_t index, loop = IDF_HEADER_LENGTH;
	uint8_t *ptr, *idf_buffer;
	uint8_t *cursor, state = STATE_CHARACTER;
	uint32_t width, height;
	uint32_t column = 0, row = 0;
	uint32_t character, attribute, foreground, background;
	uint32_t colors[16];
	uint32_t idf_sequence_length, i = 0;

	/* libgd image pointers */
	gdImagePtr canvas;

	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (ctx->length < IDF_HEADER_LENGTH + IDF_FONT_LENGTH + IDF_PALETTE_LENGTH) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	/* Get number of columns, 16-bit endian unsigned short */
	uint32_t x2 = (ctx->buffer[9] << 8) + ctx->buffer[8] + 1;

	if (x2 < 1 || x2 > 4096) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	/* process IDF */
	/* dynamically allocated memory buffer for IDF data */
	idf_buffer = malloc(2);

	if (idf_buffer == NULL) {
		ctx->error = ANSILOVE_MEMORY_ERROR;
		return -1;
	}

	while (loop < ctx->length - IDF_FONT_LENGTH - IDF_PALETTE_LENGTH) {
		cursor = &ctx->buffer[loop];

		switch (state) {
		case STATE_CHARACTER:
			if (*cursor == 1) {
				state = STATE_RLE;
				loop++;
			} else {
				ptr = realloc(idf_buffer, i + 2);
				if (ptr == NULL) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					goto error;
				}

				idf_buffer = ptr;
				idf_buffer[i] = *cursor;
				i++;
				state = STATE_ATTRIBUTE;
			}

			loop++;
			break;
		case STATE_ATTRIBUTE:
			idf_buffer[i] = *cursor;
			i++;

			state = STATE_CHARACTER;

			loop++;
			break;
		case STATE_RLE:
			/* RLE compressed data */
			idf_sequence_length = *cursor;

			if (loop + 3 >= ctx->length) {
				ctx->error = ANSILOVE_FORMAT_ERROR;
				goto error;
			}

			while (idf_sequence_length--)
			{
				/* reallocate IDF buffer memory */
				ptr = realloc(idf_buffer, i + 2);
				if (ptr == NULL) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					goto error;
				}

				idf_buffer = ptr;

				idf_buffer[i] = ctx->buffer[loop +2];
				idf_buffer[i+1] = ctx->buffer[loop + 3];
				i += 2;
			}

			loop += 4;
			state = STATE_CHARACTER;
		}
	}

	width = x2 * 8;
	height = i / 2 / 80 * 16;

	if (!width || !height) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		goto error;
	}

	/* create IDF instance */
	canvas = gdImageCreate(width, height);

	/* error output */
	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		goto error;
	}

	/* process IDF palette */
	for (loop = 0; loop < 16; loop++) {
		index = (loop * 3) + ctx->length - IDF_PALETTE_LENGTH;
		colors[loop] = gdImageColorAllocate(canvas,
		    ctx->buffer[index] << 2 | ctx->buffer[index] >> 4,
		    ctx->buffer[index + 1] << 2 | ctx->buffer[index + 1] >> 4,
		    ctx->buffer[index + 2] << 2 | ctx->buffer[index + 2] >> 4);
	}

	/* render IDF */
	for (loop = 0; loop < i; loop += 2) {
		if (column == x2) {
			column = 0;
			row++;
		}

		character = idf_buffer[loop];
		attribute = idf_buffer[loop+1];

		background = (attribute & 240) >> 4;
		foreground = attribute & 15;

		drawchar(canvas, ctx->buffer+(ctx->length - IDF_FONT_LENGTH - IDF_PALETTE_LENGTH),
		    8, 16, column, row,
		    colors[background], colors[foreground], character);

		column++;
	}

	/* create output file */
	if (output(ctx, options, canvas) != 0)
		goto error;

	free(idf_buffer);
	return 0;

error:
	free(idf_buffer);
	return -1;
}
