//
//  ansi.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#define _XOPEN_SOURCE 700
#define _NETBSD_SOURCE
#include "../ansilove.h"

#define ANSI_SEQUENCE_MAX_LENGTH 14

// Character structure
struct ansiChar {
	int32_t column;
	int32_t row;
	int32_t background;
	int32_t foreground;
	int32_t current_character;
	bool bold;
	bool italics;
	bool underline;
};

int ansilove_ansi(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	// ladies and gentlemen, it's type declaration time
	struct fontStruct fontData;

	int32_t columns = 80;

	bool ced = false;
	bool transparent = false;
	bool workbench = false;

	const char *errstr;

	// font selection
	alSelectFont(&fontData, options->font);

	// to deal with the bits flag, we declared handy bool types
	if (!strcmp(options->mode, "ced")) {
		ced = true;
	} else if (!strcmp(options->mode, "transparent")) {
		transparent = true;
	} else if (!strcmp(options->mode, "workbench")) {
		workbench = true;
	}

	// libgd image pointers
	gdImagePtr canvas;

	// ANSi processing loops
	size_t loop = 0;
	uint32_t ansi_sequence_loop;

	// character definitions
	int32_t current_character, next_character, character;
	unsigned char ansi_sequence_character;

	// default color values
	int32_t background = 0, foreground = 7;

	// text attributes
	bool bold = false, underline = false, italics = false, blink = false;

	// positions
	int32_t column = 0, row = 0, columnMax = 0, rowMax = 0;
	int32_t saved_row = 0, saved_column = 0;

	// sequence parsing variables
	uint32_t seqValue, seq_line, seq_column;
	char *seqGrab;
	char *seqTok;

	// ANSi buffer structure array definition
	int32_t structIndex = 0;
	struct ansiChar *ansi_buffer, *temp;

	// ANSi buffer dynamic memory allocation
	ansi_buffer = malloc(sizeof (struct ansiChar));

	// ANSi interpreter
	while (loop < ctx->length) {
		current_character = ctx->buffer[loop];
		next_character = ctx->buffer[loop + 1];

		if (column == 80) {
			row++;
			column = 0;
		}

		// CR + LF
		if (current_character == 13 && next_character == 10) {
			row++;
			column = 0;
			loop++;
		}

		// LF
		if (current_character == 10) {
			row++;
			column = 0;
		}

		// tab
		if (current_character == 9)
			column += 8;

		// sub
		if (current_character == 26)
			break;

		// ANSi sequence
		if (current_character == 27 && next_character == 91) {
			for (ansi_sequence_loop = 0; ansi_sequence_loop < ANSI_SEQUENCE_MAX_LENGTH; ansi_sequence_loop++) {
				ansi_sequence_character = ctx->buffer[loop + 2 + ansi_sequence_loop];

				// cursor position
				if (ansi_sequence_character == 'H' || ansi_sequence_character == 'f') {
					printf("---\n");
					// create substring from the sequence's content
					seq_line = 1;
					seq_column = 1;
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);
					printf("seqGrab: %s\n", seqGrab);

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

					// set the positions
					row = seq_line-1;
					column = seq_column-1;
					printf("Sequence H: %i,%i\n", row, column);

					seqGrab = NULL;

					loop += ansi_sequence_loop+2;
					break;
				}

				// cursor up
				if (ansi_sequence_character == 'A') {
					// create substring from the sequence's content
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);

					// now get escape sequence's position value
					int32_t seq_line = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					row -= seq_line ? seq_line : 1;

					loop += ansi_sequence_loop+2;
					break;
				}

				// cursor down
				if (ansi_sequence_character == 'B') {
					// create substring from the sequence's content
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);

					// now get escape sequence's position value
					int32_t seq_line = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					row += seq_line ? seq_line : 1;

					loop += ansi_sequence_loop+2;
					break;
				}

				// cursor forward
				if (ansi_sequence_character == 'C') {
					// create substring from the sequence's content
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);

					// now get escape sequence's position value
					int32_t seq_column = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					column += seq_column ? seq_column : 1;

					if (column > 80)
						column = 80;

					loop += ansi_sequence_loop+2;
					break;
				}

				// cursor backward
				if (ansi_sequence_character == 'D') {
					// create substring from the sequence's content
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);

					// now get escape sequence's content length
					int32_t seq_column = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					column -= seq_column ? seq_column : 1;

					if (column < 0)
						column = 0;

					loop += ansi_sequence_loop+2;
					break;
				}

				// save cursor position
				if (ansi_sequence_character == 's') {
					saved_row = row;
					saved_column = column;

					loop += ansi_sequence_loop+2;
					break;
				}

				// restore cursor position
				if (ansi_sequence_character == 'u') {
					row = saved_row;
					column = saved_column;

					loop += ansi_sequence_loop+2;
					break;
				}

				// erase display
				if (ansi_sequence_character == 'J') {
					// create substring from the sequence's content
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);

					// convert grab to an integer
					int32_t eraseDisplayInt = strtonum(seqGrab, 0, UINT32_MAX, &errstr);
					free(seqGrab);

					if (eraseDisplayInt == 2) {
						column = 0;
						row = 0;

						columnMax = 0;
						rowMax = 0;

						// reset ansi buffer
						free(ansi_buffer);
						ansi_buffer = malloc(sizeof (struct ansiChar));
						structIndex = 0;
					}
					loop += ansi_sequence_loop+2;
					break;
				}

				// set graphics mode
				if (ansi_sequence_character == 'm') {
					// create substring from the sequence's content
					seqGrab = strndup((char *)ctx->buffer + loop + 2, ansi_sequence_loop);

					while ((seqTok = strtok(seqGrab, ";")) != NULL) {
						seqValue = strtonum(seqTok, 0, UINT32_MAX, &errstr);

						if (seqValue == 0) {
							background = 0;
							foreground = 7;
							bold = false;
							underline = false;
							italics = false;
							blink = false;
						}

						if (seqValue == 1) {
							if (!workbench) {
								foreground += 8;
							}
							bold = true;
						}

						if (seqValue == 3)
							italics = true;

						if (seqValue == 4)
							underline = true;

						if (seqValue == 5)
						{
							if (!workbench)
								background += 8;

							blink = true;
						}

						if (seqValue > 29 && seqValue < 38)
						{
							foreground = seqValue - 30;

							if (bold)
								foreground += 8;
						}

						if (seqValue > 39 && seqValue < 48)
						{
							background = seqValue - 40;

							if (blink && options->icecolors)
								background += 8;
						}

						seqGrab = NULL;
					}

					loop += ansi_sequence_loop+2;
					break;
				}

				// cursor (de)activation (Amiga ANSi)
				if (ansi_sequence_character == 'p') {
					loop += ansi_sequence_loop+2;
					break;
				}

				// skipping set mode and reset mode sequences
				if (ansi_sequence_character == 'h' || ansi_sequence_character == 'l') {
					loop += ansi_sequence_loop+2;
					break;
				}

				// skipping erase in line (EL) sequences
				if (ansi_sequence_character == 'K') {
					loop += ansi_sequence_loop+2;
					break;
				}

				// skipping PabloDraw 24-bit ANSI sequences
				if (ansi_sequence_character == 't') {
					loop += ansi_sequence_loop+2;
					break;
				}
			}
		}
		else if (current_character != 10 && current_character != 13 && current_character != 9)
		{
			// record number of columns and lines used
			if (column > columnMax)
				columnMax = column;

			if (row > rowMax)
				rowMax = row;

			// write current character in ansiChar structure
			if (!fontData.isAmigaFont || (current_character != 12 && current_character != 13)) {
				// reallocate structure array memory
				temp = realloc(ansi_buffer, (structIndex + 1) * sizeof (struct ansiChar));
				ansi_buffer = temp;

				ansi_buffer[structIndex].background = background;
				ansi_buffer[structIndex].foreground = foreground;
				ansi_buffer[structIndex].current_character = current_character;
				ansi_buffer[structIndex].bold = bold;
				ansi_buffer[structIndex].italics = italics;
				ansi_buffer[structIndex].underline = underline;
				ansi_buffer[structIndex].column = column;
				ansi_buffer[structIndex].row = row;

				structIndex++;
				column++;
			}
		}
		loop++;
	}

	// allocate image buffer memory
	columnMax++;
	rowMax++;

	if (ced)
		columns = 78;

	if (options->diz)
		columns = fmin(columnMax, 80);

	// create that damn thingy
	canvas = gdImageCreate(columns * options->bits, (rowMax)*fontData.height);

	if (!canvas) {
		ctx->error = GD_ERROR;
		return -1;
	}

	int32_t colors[16];

	int32_t ced_background = 0, ced_foreground = 0;

	if (ced) {
		ced_background = gdImageColorAllocate(canvas, 170, 170, 170);
		ced_foreground = gdImageColorAllocate(canvas, 0, 0, 0);
		gdImageFill(canvas, 0, 0, ced_background);
	} else if (workbench) {
		gdImageFill(canvas, 0, 0, 0);

		for (int i = 0; i < 16; i++) {
			colors[i] = gdImageColorAllocate(canvas, workbench_palette[i*3],
			    workbench_palette[i*3+1],
			    workbench_palette[i*3+2]);
		}
	} else {
		// Allocate standard ANSi color palette

		for (int i = 0; i < 16; i++) {
			colors[i] = gdImageColorAllocate(canvas, ansi_palette[i*3],
			    ansi_palette[i*3+1],
			    ansi_palette[i*3+2]);
		}
	}

	// even more definitions, sigh
	uint32_t ansiBufferItems = structIndex;

	// render ANSi
	for (loop = 0; loop < ansiBufferItems; loop++) {
		// grab ANSi char from our structure array
		background = ansi_buffer[loop].background;
		foreground = ansi_buffer[loop].foreground;
		character = ansi_buffer[loop].current_character;
		bold = ansi_buffer[loop].bold;
		italics = ansi_buffer[loop].italics;
		underline = ansi_buffer[loop].underline;
		column = ansi_buffer[loop].column;
		row = ansi_buffer[loop].row;

		if (ced) {
			drawchar(canvas, fontData.font_data, options->bits, fontData.height,
			    column, row, ced_background, ced_foreground, character);
		} else {
			drawchar(canvas, fontData.font_data, options->bits, fontData.height,
			    column, row, colors[background], colors[foreground], character);
		}

	}

	// transparent flag used?
	if (transparent)
		gdImageColorTransparent(canvas, 0);

	// create output image
	output(ctx, canvas, options->retinaScaleFactor);

	// free memory
	free(ansi_buffer);

	return 0;
}
