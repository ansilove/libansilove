/*
 * pcboard.c
 * libansilove 1.1.2
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2019 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <gd.h>
#include <stdint.h>
#include <stdlib.h>
#include "ansilove.h"
#include "config.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#define LF	'\n'
#define CR	'\r'
#define TAB	'\t'
#define SUB	26

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
	uint32_t loop, structIndex;

	/* font selection */
	alSelectFont(&fontData, options->font);

	/* libgd image pointers */
	gdImagePtr canvas;

	uint8_t character, *cursor;
	uint32_t background = 0, foreground = 7;
	uint32_t column = 0, row = 0, columnMax = 0, rowMax = 0;

	/* PCB buffer structure array definition */
	struct pcbChar *ptr, *pcboard_buffer;

	/* PCB buffer dynamic memory allocation */
	pcboard_buffer = malloc(sizeof (struct pcbChar));

	/* reset loop */
	loop = 0;
	structIndex = 0;

	while (loop < ctx->length) {
		cursor = &ctx->buffer[loop];

		if (column == 80) {
			row++;
			column = 0;
		}

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
			loop = ctx->length;
			break;
		case '@':
			/* PCB sequence */
			if (*++cursor == 'X') {
				/* set graphics rendition */
				background = ctx->buffer[loop+2];
				foreground = ctx->buffer[loop+3];
				loop += 3;
			}

			if (*cursor == 'C' && ctx->buffer[loop+2] == 'L'
			    && ctx->buffer[loop+3] == 'S') {
				/* erase display */
				column = 0;
				row = 0;

				columnMax = 0;
				rowMax = 0;

				loop += 4;
			}

			if (*cursor == 'P' && ctx->buffer[loop+2] == 'O'
			    && ctx->buffer[loop+3] == 'S' && ctx->buffer[loop+4] == ':') {
				/* cursor position */
				if (ctx->buffer[loop+6] == '@')
				{
					column = ((ctx->buffer[loop+5])-48)-1;
					loop += 5;
				} else {
					column = (10 * ((ctx->buffer[loop+5])-48) + (ctx->buffer[loop+6])-48)-1;
					loop += 6;
				}
			}
			break;
		default:
			/* record number of columns and lines used */
			if (column > columnMax)
				columnMax = column;

			if (row > rowMax)
				rowMax = row;

			/* reallocate structure array memory */
			ptr = realloc(pcboard_buffer, (structIndex + 1) * sizeof (struct pcbChar));
			if (ptr == NULL) {
				ctx->error = ANSILOVE_MEMORY_ERROR;
				free(pcboard_buffer);
				return -1;
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
		}

		loop++;
	}
	columnMax++;
	rowMax++;

	/* allocate buffer image memory */
	canvas = gdImageCreate(80 * options->bits, (rowMax)*fontData.height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		free(pcboard_buffer);
		return -1;
	}

	/* allocate color palette */
	uint32_t colors[16];

	for (int i = 0; i < 16; i++) {
		colors[i] = gdImageColorAllocate(canvas, ansi_palette[i*3],
		    ansi_palette[i*3+1],
		    ansi_palette[i*3+2]);
	}

	uint32_t pcbBufferItems = structIndex;

	/* render PCB */
	for (loop = 0; loop < pcbBufferItems; loop++) {
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
	if (output(ctx, options, canvas) != 0) {
		free(pcboard_buffer);
		return -1;
	}

	free(pcboard_buffer);

	return 0;
}
