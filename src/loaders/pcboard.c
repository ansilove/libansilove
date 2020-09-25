/*
 * pcboard.c
 * libansilove 1.2.5
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
	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	struct fontStruct fontData;
	size_t loop = 0, structIndex = 0;

	options->columns = options->columns ? options->columns : 80;
	uint16_t columns = options->columns;

	/* font selection */
	memset(&fontData, 0, sizeof(struct fontStruct));
	select_font(&fontData, options->font);

	/* libgd image pointers */
	gdImagePtr canvas;

	uint8_t character, *cursor, state = STATE_TEXT;
	uint32_t background = '0', foreground = '7';
	uint32_t column = 0, row = 0, rowMax = 0;

	/* PCB buffer structure array definition */
	struct pcbChar *ptr, *pcboard_buffer;

	/* PCB buffer dynamic memory allocation */
	pcboard_buffer = malloc(sizeof (struct pcbChar));

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
				ptr = realloc(pcboard_buffer, (structIndex + 1) * sizeof (struct pcbChar));
				if (ptr == NULL) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					goto error;
				} else {
					pcboard_buffer = ptr;
				}

				/* write current character in pcbChar structure */
				pcboard_buffer[structIndex].column = column;
				pcboard_buffer[structIndex].row = row;
				pcboard_buffer[structIndex].background = pcb_colors[background];
				pcboard_buffer[structIndex].foreground = pcb_colors[foreground];
				pcboard_buffer[structIndex].character = *cursor;

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

	/* allocate buffer image memory */
	canvas = gdImageCreate(columns * options->bits, rowMax*fontData.height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		goto error;
	}

	/* allocate color palette */
	uint32_t colors[16];

	for (size_t i = 0; i < 16; i++)
		colors[i] = gdImageColorAllocate(canvas, ansi_palette_red[i],
		    ansi_palette_green[i], ansi_palette_blue[i]);

	/* render PCB */
	for (loop = 0; loop < structIndex; loop++) {
		/* grab our chars out of the structure */
		column = pcboard_buffer[loop].column;
		row = pcboard_buffer[loop].row;
		background = pcboard_buffer[loop].background;
		foreground = pcboard_buffer[loop].foreground;
		character = pcboard_buffer[loop].character;

		drawchar(canvas, fontData.font_data, options->bits, fontData.height,
		    column, row, colors[background], colors[foreground], character);
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
