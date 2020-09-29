/*
 * ansi.c
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
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"
#include "config.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#ifndef HAVE_STRTONUM
#include "strtonum.h"
#endif

#ifndef HAVE_REALLOCARRAY
#include "reallocarray.h"
#endif

#define ANSI_SEQUENCE_MAX_LENGTH 14
#define ANSI_BUFFER_SIZE 65536

#define STATE_TEXT	0
#define STATE_SEQUENCE	1
#define STATE_END	2

/* Character structure */
struct ansiChar {
	int32_t column;
	int32_t row;
	uint32_t background;
	uint32_t foreground;
	uint8_t character;
};

int
ansilove_ansi(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	/* ladies and gentlemen, it's type declaration time */
	struct fontStruct fontData;

	/* Default to 80 columns if columns option wasn't set */
	options->columns = options->columns ? options->columns : 80;

	int16_t columns = options->columns;

	bool ced = false;
	bool workbench = false;

	const char *errstr;

	/* font selection */
	memset(&fontData, 0, sizeof(struct fontStruct));
	select_font(&fontData, options->font);

	switch (options->mode) {
	case ANSILOVE_MODE_CED:
		ced = true;
		break;
	case ANSILOVE_MODE_WORKBENCH:
		workbench = true;
		break;
	}

	/* libgd image pointers */
	gdImagePtr canvas;

	/* ANSi processing loops */
	size_t loop = 0, ansi_sequence_loop;

	/* character definitions */
	uint8_t character;
	uint8_t *cursor, state = STATE_TEXT;
	uint8_t ansi_sequence_character;

	/* default color values */
	uint32_t background = 0, foreground = 7;
	uint32_t background24 = 0, foreground24 = 0;

	/* text attributes */
	bool bold = false, blink = false, invert = false;

	/* positions */
	int32_t column = 0, row = 0, columnMax = 0, rowMax = 0;
	int32_t saved_row = 0, saved_column = 0;

	/* sequence parsing variables */
	uint32_t maxlength;
	uint32_t seqValue, seq_line, seq_column;
	char *seqGrab = NULL;
	char *seqTok = NULL;

	/* ANSi buffer structure array definition */
	size_t structIndex = 0;
	struct ansiChar *ptr, *ansi_buffer;

	size_t ansi_buffer_size = ANSI_BUFFER_SIZE;

	/* ANSi buffer dynamic memory allocation */
	ansi_buffer = malloc(ansi_buffer_size * sizeof(struct ansiChar));

	if (ansi_buffer == NULL) {
		ctx->error = ANSILOVE_MEMORY_ERROR;
		return -1;
	}

	/* ANSi interpreter */
	while (loop < ctx->length) {
		cursor = &ctx->buffer[loop];

		if (column == options->columns) {
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
			case ESC:
				/* ANSi sequence */
				if ((loop+1 < ctx->length) && ctx->buffer[loop + 1] == 91) {
					state = STATE_SEQUENCE;
					loop++;
				}
				break;
			default:
				/* record number of columns and lines used */
				if (column > columnMax)
					columnMax = column;

				if (row > rowMax)
					rowMax = row;

				/* write current character in ansiChar structure */
				/* reallocate structure array memory */
				if (structIndex == ansi_buffer_size) {
					ansi_buffer_size += ANSI_BUFFER_SIZE;

					ptr = reallocarray(ansi_buffer, ansi_buffer_size, sizeof(struct ansiChar));

					if (ptr == NULL) {
						ctx->error = ANSILOVE_MEMORY_ERROR;
						free(ansi_buffer);
						return -1;
					}

					ansi_buffer = ptr;
				}

				if (invert) {
					ansi_buffer[structIndex].background = foreground % 8;
					ansi_buffer[structIndex].foreground = background + (foreground & 8);
				} else {
					ansi_buffer[structIndex].background =
					    background24 ? background24 : background;

					ansi_buffer[structIndex].foreground =
					    foreground24 ? foreground24 : foreground;
				}
				ansi_buffer[structIndex].character = *cursor;
				ansi_buffer[structIndex].column = column;
				ansi_buffer[structIndex].row = row;

				structIndex++;
				column++;
			}
			break;
		case STATE_SEQUENCE:
			maxlength = fmin(ctx->length - loop, ANSI_SEQUENCE_MAX_LENGTH);
			for (ansi_sequence_loop = 0; ansi_sequence_loop < maxlength; ansi_sequence_loop++) {
				ansi_sequence_character = ctx->buffer[loop + ansi_sequence_loop];

				/* cursor position */
				if (ansi_sequence_character == 'H' || ansi_sequence_character == 'f') {
					/* create substring from the sequence's content */
					seq_line = 1;
					seq_column = 1;
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					if (!strncmp(seqGrab, ";", 1)) {
						seq_line = 1;
						seqTok = strtok(seqGrab, ";");

						if (seqTok)
							seq_column = strtonum(seqTok, 0, UINT32_MAX, &errstr);
					} else {
						seqTok = strtok(seqGrab, ";");
						if (seqTok)
							seq_line = strtonum(seqTok, 0, UINT32_MAX, &errstr);

						seqTok = strtok(NULL, ";");
						if (seqTok)
							seq_column = strtonum(seqTok, 0, UINT32_MAX, &errstr);
					}

					/* set the positions */
					row = seq_line-1;
					column = seq_column-1;

					loop += ansi_sequence_loop;
					free(seqGrab);
					break;
				}

				/* cursor up */
				if (ansi_sequence_character == 'A') {
					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					/* now get escape sequence's position value */
					uint32_t seq_line = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					row -= seq_line ? seq_line : 1;

					if (row < 0)
						row = 0;

					loop += ansi_sequence_loop;
					break;
				}

				/* cursor down */
				if (ansi_sequence_character == 'B') {
					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					/* now get escape sequence's position value */
					uint32_t seq_line = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					row += seq_line ? seq_line : 1;

					loop += ansi_sequence_loop;
					break;
				}

				/* cursor forward */
				if (ansi_sequence_character == 'C') {
					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					/* now get escape sequence's position value */
					uint32_t seq_column = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					column += seq_column ? seq_column : 1;

					if (column > options->columns)
						column = options->columns;

					loop += ansi_sequence_loop;
					break;
				}

				/* cursor backward */
				if (ansi_sequence_character == 'D') {
					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					/* now get escape sequence's content length */
					uint32_t seq_column = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					column -= seq_column ? seq_column : 1;

					if (column < 0)
						column = 0;

					loop += ansi_sequence_loop;
					break;
				}

				/* save cursor position */
				if (ansi_sequence_character == 's') {
					saved_row = row;
					saved_column = column;

					loop += ansi_sequence_loop;
					break;
				}

				/* restore cursor position */
				if (ansi_sequence_character == 'u') {
					row = saved_row;
					column = saved_column;

					loop += ansi_sequence_loop;
					break;
				}

				/* erase display */
				if (ansi_sequence_character == 'J') {
					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					/* convert grab to an integer */
					uint32_t eraseDisplayInt = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					if (eraseDisplayInt == 2) {
						column = 0;
						row = 0;

						columnMax = 0;
						rowMax = 0;

						/* reset ansi buffer */
						structIndex = 0;
					}
					loop += ansi_sequence_loop;
					break;
				}

				/* set graphics mode */
				if (ansi_sequence_character == 'm') {
					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					seqTok = strtok(seqGrab, ";");
					while (seqTok) {
						seqValue = strtonum(seqTok, 0, UINT32_MAX, &errstr);

						if (seqValue == 0) {
							background = 0;
							background24 = 0;
							foreground = 7;
							foreground24 = 0;
							bold = false;
							blink = false;
							invert = false;
						}

						if (seqValue == 1) {
							if (!workbench) {
								foreground += 8;
							}
							bold = true;
							foreground24 = 0;
						}

						if (seqValue == 5) {
							if (!workbench && options->icecolors)
								background += 8;

							blink = true;
							background24 = 0;
						}

						if (seqValue == 7)
							invert = true;

						if (seqValue == 27)
							invert = false;

						if (seqValue > 29 && seqValue < 38) {
							foreground = seqValue - 30;
							foreground24 = 0;

							if (bold)
								foreground += 8;
						}

						if (seqValue > 39 && seqValue < 48) {
							background = seqValue - 40;
							background24 = 0;

							if (blink && options->icecolors)
								background += 8;
						}

						seqTok = strtok(NULL, ";");
					}

					loop += ansi_sequence_loop;
					free(seqGrab);
					break;
				}

				/* cursor (de)activation (Amiga ANSi) */
				if (ansi_sequence_character == 'p') {
					loop += ansi_sequence_loop;
					break;
				}

				/* skipping set mode and reset mode sequences */
				if (ansi_sequence_character == 'h' || ansi_sequence_character == 'l') {
					loop += ansi_sequence_loop;
					break;
				}

				/* skipping erase in line (EL) sequences */
				if (ansi_sequence_character == 'K') {
					loop += ansi_sequence_loop;
					break;
				}

				/* PabloDraw 24-bit ANSI sequences */
				if (ansi_sequence_character == 't') {
					uint32_t color_R = 0, color_G = 0, color_B = 0;

					/* create substring from the sequence's content */
					seqGrab = strndup((char *)cursor, ansi_sequence_loop);

					seqTok = strtok(seqGrab, ";");
					if (seqTok) {
						seqValue = strtonum(seqTok, 0, UCHAR_MAX, &errstr);

						seqTok = strtok(NULL, ";");
						color_R = seqTok ? strtonum(seqTok, 0, UCHAR_MAX, &errstr) & 0xff : 0;
						seqTok = strtok(NULL, ";");
						color_G = seqTok ? strtonum(seqTok, 0, UCHAR_MAX, &errstr) & 0xff : 0;
						seqTok = strtok(NULL, ";");
						color_B = seqTok ? strtonum(seqTok, 0, UCHAR_MAX, &errstr) & 0xff : 0;

						switch (seqValue) {
						case 0:
							background24 = color_R << 16 | color_G << 8 | color_B;
							break;
						case 1:
							foreground24 = color_R << 16 | color_G << 8 | color_B;
							break;
						}

						options->truecolor = true;
					}

					loop += ansi_sequence_loop;
					free(seqGrab);
					break;
				}
			}
			state = STATE_TEXT;
			break;
		case STATE_END:
			loop = ctx->length;
			break;
		}

		loop++;
	}

	/* allocate image buffer memory */
	columnMax++;
	rowMax++;

	if (ced)
		columns = 78;

	if (options->diz)
		columns = fmin(columnMax, options->columns);

	uint32_t width = columns * options->bits;
	uint32_t height = rowMax * fontData.height;

	if (!width || !height) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		free(ansi_buffer);
		return -1;
	}

	/* create that damn thingy */
	canvas = options->truecolor ?
	    gdImageCreateTrueColor(width, height) :
	    gdImageCreate(width, height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		free(ansi_buffer);
		return -1;
	}

	uint32_t colors[16];

	uint32_t ced_background = 0, ced_foreground = 0;

	if (ced) {
		ced_background = gdImageColorAllocate(canvas, 170, 170, 170);
		ced_foreground = gdImageColorAllocate(canvas, 0, 0, 0);
		gdImageFill(canvas, 0, 0, ced_background);
	} else if (workbench) {
		for (size_t i = 0; i < 16; i++)
			colors[i] = gdImageColorAllocate(canvas,
			    workbench_palette_red[i],
			    workbench_palette_green[i],
			    workbench_palette_blue[i]);
	} else {
		/* Allocate standard ANSi color palette */

		for (size_t i = 0; i < 16; i++)
			colors[i] = gdImageColorAllocate(canvas,
			    ansi_palette_red[i], ansi_palette_green[i],
			    ansi_palette_blue[i]);
	}

	/* render ANSi */
	for (loop = 0; loop < structIndex; loop++) {
		/* grab ANSi char from our structure array */
		background = ansi_buffer[loop].background;
		foreground = ansi_buffer[loop].foreground;
		character = ansi_buffer[loop].character;
		column = ansi_buffer[loop].column;
		row = ansi_buffer[loop].row;

		if (ced) {
			background = ced_background;
			foreground = ced_foreground;
		} else {
			if (background < 16)
				background = colors[background];

			if (foreground < 16)
				foreground = colors[foreground];
		}

		drawchar(canvas, fontData.font_data, options->bits,
		    fontData.height, column, row, background, foreground,
		    character);
	}

	/* create output image */
	if (output(ctx, options, canvas) != 0) {
		free(ansi_buffer);
		return -1;
	}

	/* free memory */
	free(ansi_buffer);

	return 0;
}
