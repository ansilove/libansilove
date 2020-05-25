/*
 * icedraw.c
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
#include <stdint.h>
#include <stdlib.h>
#include "ansilove.h"
#include "drawchar.h"
#include "output.h"

#define IDF_HEADER_LENGTH 12
#define IDF_FONT_LENGTH 4096
#define IDF_PALETTE_LENGTH 48

int
ansilove_icedraw(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
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

	/* libgd image pointers */
	gdImagePtr canvas;

	uint32_t loop = IDF_HEADER_LENGTH;
	uint32_t index;
	uint32_t colors[16];

	/* process IDF */
	uint32_t idf_sequence_length, i = 0;

	/* dynamically allocated memory buffer for IDF data */
	uint8_t *ptr, *idf_buffer;
	idf_buffer = malloc(2);

	while (loop < ctx->length - IDF_FONT_LENGTH - IDF_PALETTE_LENGTH) {
		/* RLE compressed data */
		if (ctx->buffer[loop] == 1) {
			idf_sequence_length = ctx->buffer[loop+2];

			while (idf_sequence_length--)
			{
				/* reallocate IDF buffer memory */
				ptr = realloc(idf_buffer, i + 2);
				if (ptr == NULL) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					free(idf_buffer);
					return -1;
				} else {
					idf_buffer = ptr;
				}

				idf_buffer[i] = ctx->buffer[loop + 4];
				idf_buffer[i+1] = ctx->buffer[loop + 5];
				i += 2;
			}
			loop += 4;
		} else {
			/* reallocate IDF buffer memory */
			ptr = realloc(idf_buffer, i + 2);
			if (ptr == NULL) {
				ctx->error = ANSILOVE_MEMORY_ERROR;
				free(idf_buffer);
				return -1;
			} else {
				idf_buffer = ptr;
			}

			/* normal character */
			idf_buffer[i] = ctx->buffer[loop];
			idf_buffer[i+1] = ctx->buffer[loop + 1];
			i += 2;
		}
		loop += 2;
	}

	/* create IDF instance */
	canvas = gdImageCreate(x2 * 8, i / 2 / 80 * 16);

	/* error output */
	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		free(idf_buffer);
		return -1;
	}

	/* process IDF palette */
	for (loop = 0; loop < 16; loop++) {
		index = (loop * 3) + ctx->length - IDF_PALETTE_LENGTH;
		colors[loop] = gdImageColorAllocate(canvas,
		    (ctx->buffer[index] << 2 | ctx->buffer[index] >> 4),
		    (ctx->buffer[index + 1] << 2 | ctx->buffer[index + 1] >> 4),
		    (ctx->buffer[index + 2] << 2 | ctx->buffer[index + 2] >> 4));
	}

	/* render IDF */
	uint32_t column = 0, row = 0;
	uint32_t character, attribute, foreground, background;

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
	if (output(ctx, options, canvas) != 0) {
		free(idf_buffer);
		return -1;
	}

	free(idf_buffer);

	return 0;
}
