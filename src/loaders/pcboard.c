/*
 * pcboard.c
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"
#include "config.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#ifndef HAVE_REALLOCARRAY
#include "reallocarray.h"
#endif

#define STATE_TEXT	0
#define STATE_SEQUENCE	1
#define STATE_END	2

/* Character structure */
struct pcbChar {
	uint32_t column;
	uint32_t row;
	uint32_t background;
	uint32_t foreground;
	uint8_t character;
};

int
ansilove_pcboard(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	uint8_t *cursor, state = STATE_TEXT;
	uint32_t background = '0', foreground = '7';
	uint32_t column = 0, row = 0, rowMax = 0;
	uint32_t width, height;
	uint32_t colors[16];
	size_t loop = 0, structIndex = 0;
	struct fontStruct fontData;

	/* PCB buffer structure array definition */
	struct pcbChar *ptr, *pcboard_buffer = NULL;

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

	if (options->bits != 8 && options->bits !=9) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	options->columns = options->columns ? options->columns : 80;
	uint16_t columns = options->columns;

	if (columns < 1 || columns > 4096) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	/* font selection */
	memset(&fontData, 0, sizeof(struct fontStruct));
	select_font(&fontData, options->font);

	/* PCB buffer dynamic memory allocation */
	pcboard_buffer = malloc(sizeof (struct pcbChar));

	if (pcboard_buffer == NULL) {
		ctx->error = ANSILOVE_MEMORY_ERROR;
		return -1;
	}

	while (loop < ctx->length) {
		cursor = &ctx->buffer[loop];

		if (column == columns) {
			row++;
			column = 0;
		}

		switch (state) {
		case STATE_TEXT:
			switch (*cursor) {
			case LF:
				row++;
				column = 0;
				break;
			case CR:
				break;
			case TAB:
				column += 8;
				break;
			case SUB:
				state = STATE_END;
				break;
			case '@':
				/* PCB sequence */
				state = STATE_SEQUENCE;
				break;
			default:
				/* record number of lines used */
				if (row > rowMax)
					rowMax = row;

				/* reallocate structure array memory */
				ptr = reallocarray(pcboard_buffer, structIndex + 1, sizeof(struct pcbChar));
				if (ptr == NULL) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					goto error;
				}

				pcboard_buffer = ptr;

				/* write current character in pcbChar struct */
				pcboard_buffer[structIndex] = (struct pcbChar) {
					.column = column,
					.row = row,
					.background = pcb_colors[background],
					.foreground = pcb_colors[foreground],
					.character = *cursor
				};

				column++;
				structIndex++;
			};
			loop++;
			break;
		case STATE_SEQUENCE:
			if (*cursor == 'X') {
				/* set graphics rendition */
				if (loop + 2 < ctx->length) {
					background = *++cursor;
					foreground = *++cursor;

					loop += 3;
				}

				if (background >= PCB_COLORS ||
				    foreground >= PCB_COLORS) {
					ctx->error = ANSILOVE_FORMAT_ERROR;
					goto error;
				}
			}

			if (loop + 3 < ctx->length && !memcmp(cursor, "CLS@", 4)) {
				/* erase display */
				column = 0;
				row = 0;

				rowMax = 0;

				/* reset pcboard buffer */
				structIndex = 0;

				loop += 4;
			}

			state = STATE_TEXT;
			break;
		case STATE_END:
			loop = ctx->length;
			break;
		}
	}
	rowMax++;

	width = columns * options->bits;
	height = rowMax * fontData.height;

	if (!width || !height) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		goto error;
	}

	/* allocate buffer image memory */
	canvas = gdImageCreate(width, height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		goto error;
	}

	/* allocate color palette */
	for (size_t i = 0; i < 16; i++)
		colors[i] = gdImageColorAllocate(canvas, ansi_palette_red[i],
		    ansi_palette_green[i], ansi_palette_blue[i]);

	/* render PCB */
	for (loop = 0; loop < structIndex; loop++) {
		drawchar(canvas, fontData.font_data, options->bits, fontData.height,
		    pcboard_buffer[loop].column,
		    pcboard_buffer[loop].row,
		    colors[pcboard_buffer[loop].background],
		    colors[pcboard_buffer[loop].foreground],
		    pcboard_buffer[loop].character);
	}

	/* create output image */
	if (output(ctx, options, canvas) != 0)
		goto error;

	free(pcboard_buffer);
	return 0;

error:
	free(pcboard_buffer);
	return -1;
}
