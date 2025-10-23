/*
 * terminal.c
 * libansilove 1.4.2
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

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"
#include "config.h"
#include "cp437_unicode.h"
#include "dos_colors.h"

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
#define STATE_SEQUENCE_PARAM	2
#define STATE_END	3

struct terminal_cell {
	uint8_t character;
	uint32_t foreground;
	uint32_t background;
	bool bold;
	bool blink;
	bool invert;
};

struct terminal_grid {
	struct terminal_cell **cells;
	int32_t max_column;
	int32_t max_row;
	int32_t width;
	int32_t height;
};

static struct terminal_grid *
terminal_grid_create(int32_t width, int32_t height)
{
	struct terminal_grid *grid;
	int32_t row;

	grid = malloc(sizeof(struct terminal_grid));
	if (!grid)
		return NULL;

	grid->cells = malloc(height * sizeof(struct terminal_cell *));
	if (!grid->cells) {
		free(grid);
		return NULL;
	}

	for (row = 0; row < height; row++) {
		grid->cells[row] = calloc(width, sizeof(struct terminal_cell));
		if (!grid->cells[row]) {
			for (int32_t i = 0; i < row; i++)
				free(grid->cells[i]);
			free(grid->cells);
			free(grid);
			return NULL;
		}
	}

	grid->width = width;
	grid->height = height;
	grid->max_row = -1;
	grid->max_column = -1;

	return grid;
}

static void
terminal_grid_free(struct terminal_grid *grid)
{
	if (!grid)
		return;

	for (int32_t row = 0; row < grid->height; row++)
		free(grid->cells[row]);
	free(grid->cells);
	free(grid);
}

static void
terminal_grid_set_cell(struct terminal_grid *grid, int32_t col, int32_t row,
		       uint8_t ch, uint32_t fg, uint32_t bg,
		       bool bold, bool blink, bool invert)
{
	if (col < 0 || col >= grid->width || row < 0 || row >= grid->height)
		return;

	grid->cells[row][col].character = ch;
	grid->cells[row][col].foreground = fg;
	grid->cells[row][col].background = bg;
	grid->cells[row][col].bold = bold;
	grid->cells[row][col].blink = blink;
	grid->cells[row][col].invert = invert;

	if (col > grid->max_column)
		grid->max_column = col;
	if (row > grid->max_row)
		grid->max_row = row;
}

static int
terminal_emit_cell(uint8_t **out, size_t *out_len, size_t *out_pos,
		   struct terminal_cell *cell, struct terminal_cell *prev_cell)
{
	uint8_t utf8_char[4];
	int utf8_len;
	int sgr_len;
	char sgr[32];
	uint8_t ansi_code;
	bool needs_reset = false;

	if (!out || !*out || !out_len || !out_pos)
		return -1;

	if (*out_pos >= *out_len - 20)
		return -2;

	if (prev_cell && (cell->foreground != prev_cell->foreground ||
			  cell->background != prev_cell->background ||
			  cell->bold != prev_cell->bold ||
			  cell->blink != prev_cell->blink ||
			  cell->invert != prev_cell->invert)) {
		needs_reset = true;
	} else if (!prev_cell) {
		needs_reset = true;
	}

	if (needs_reset) {
		(*out)[(*out_pos)++] = '\033';
		(*out)[(*out_pos)++] = '[';
		(*out)[(*out_pos)++] = '0';
		(*out)[(*out_pos)++] = 'm';

		if (cell->invert) {
			(*out)[(*out_pos)++] = '\033';
			(*out)[(*out_pos)++] = '[';
			(*out)[(*out_pos)++] = '7';
			(*out)[(*out_pos)++] = 'm';
		}

		if (cell->bold) {
			(*out)[(*out_pos)++] = '\033';
			(*out)[(*out_pos)++] = '[';
			(*out)[(*out_pos)++] = '1';
			(*out)[(*out_pos)++] = 'm';
		}

		if (cell->blink) {
			(*out)[(*out_pos)++] = '\033';
			(*out)[(*out_pos)++] = '[';
			(*out)[(*out_pos)++] = '5';
			(*out)[(*out_pos)++] = 'm';
		}

		if (!cell->invert) {
			const struct rgb_color *fg_rgb = &dos_palette[cell->foreground];
			sgr_len = snprintf(sgr, sizeof(sgr), "\033[38;2;%d;%d;%dm",
					   fg_rgb->r, fg_rgb->g, fg_rgb->b);
			if (sgr_len > 0 && sgr_len < (int)sizeof(sgr)) {
				if (*out_pos + sgr_len >= *out_len)
					return -2;
				memcpy(*out + *out_pos, sgr, sgr_len);
				*out_pos += sgr_len;
			}
		}

		const struct rgb_color *bg_rgb = &dos_palette[cell->background];
		sgr_len = snprintf(sgr, sizeof(sgr), "\033[48;2;%d;%d;%dm",
				   bg_rgb->r, bg_rgb->g, bg_rgb->b);
		if (sgr_len > 0 && sgr_len < (int)sizeof(sgr)) {
			if (*out_pos + sgr_len >= *out_len)
				return -2;
			memcpy(*out + *out_pos, sgr, sgr_len);
			*out_pos += sgr_len;
		}
	}

	utf8_len = cp437_to_utf8(cell->character, utf8_char);
	if (utf8_len <= 0 || utf8_len > 4)
		return -1;

	if (*out_pos + utf8_len >= *out_len)
		return -2;

	memcpy(*out + *out_pos, utf8_char, utf8_len);
	*out_pos += utf8_len;

	return 0;
}

int
ansilove_terminal(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	const char *errstr;

	size_t loop = 0, ansi_sequence_loop = 0;
	uint8_t character;
	uint8_t *cursor, state = STATE_TEXT;
	uint8_t ansi_sequence_character;

	uint32_t background = 0, foreground = 7;
	uint32_t colors[16];

	bool bold = false, blink = false, invert = false;

	int32_t column = 0, row = 0;
	int32_t saved_row = 0, saved_column = 0;

	uint32_t seqValue, seq_line, seq_column;
	char *seqGrab = NULL;
	char *seqTok = NULL;

	struct terminal_grid *grid;
	uint8_t *old_buffer;
	size_t old_length;

	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (!ctx->length) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	uint32_t columns = 80;
	if (options->columns > 0)
		columns = options->columns;

	grid = terminal_grid_create(columns, 500);
	if (!grid) {
		ctx->error = ANSILOVE_MEMORY_ERROR;
		return -1;
	}

	old_buffer = ctx->buffer;
	old_length = ctx->length;
	cursor = old_buffer;

	dos_palette_init(colors);

	while (loop < old_length) {
		character = *cursor++;
		loop++;

		switch (state) {
		case STATE_TEXT:
			if (character == 0x1B) {
				state = STATE_SEQUENCE;
			} else if (character == 0x0D) {
				column = 0;
			} else if (character == 0x0A) {
				if (column > grid->max_column)
					grid->max_column = column;
				row++;
				column = 0;

				if (row >= grid->height - 1)
					state = STATE_END;
			} else if (character >= 0x20) {
				uint32_t actual_fg = foreground;
				if (bold && foreground < 8)
					actual_fg = foreground + 8;
				
				terminal_grid_set_cell(grid, column, row, character,
						       actual_fg, background,
						       bold, blink, invert);
				column++;

				if (column >= (int32_t)columns) {
					column = 0;
					row++;

					if (row >= grid->height - 1)
						state = STATE_END;
				}
			}
			break;

		case STATE_SEQUENCE:
			if (character == '[') {
				seqGrab = malloc(ANSI_SEQUENCE_MAX_LENGTH);
				if (!seqGrab) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					terminal_grid_free(grid);
					return -1;
				}

			memset(seqGrab, 0, ANSI_SEQUENCE_MAX_LENGTH);
			ansi_sequence_loop = 0;
			state = STATE_SEQUENCE_PARAM;
			} else {
				state = STATE_TEXT;
			}
			break;

		default:
			if ((character >= 0x30 && character <= 0x3F) ||
			    (character >= 0x20 && character <= 0x2F)) {
				if (ansi_sequence_loop < ANSI_SEQUENCE_MAX_LENGTH - 1) {
					seqGrab[ansi_sequence_loop] = character;
					ansi_sequence_loop++;
				}
			} else if (character >= 0x40 && character <= 0x7E) {
				ansi_sequence_character = character;

				if (ansi_sequence_character == 'H' ||
				    ansi_sequence_character == 'f') {
					seqTok = strtok(seqGrab, ";");

					if (seqTok) {
						seqValue = strtonum(seqTok,
								    0, INT_MAX, &errstr);

						if (seqValue == 0)
							seq_line = 0;
						else
							seq_line = seqValue - 1;
					}

					seqTok = strtok(NULL, ";");

					if (seqTok) {
						seqValue = strtonum(seqTok,
								    0, INT_MAX, &errstr);

						if (seqValue == 0)
							seq_column = 0;
						else
							seq_column = seqValue - 1;
					}

					row = seq_line;
					column = seq_column;
				} else if (ansi_sequence_character == 'A') {
					seqValue = strtonum(seqGrab, 0, INT_MAX,
							    &errstr);

					if (seqValue)
						row -= seqValue;

					if (row < 0)
						row = 0;
				} else if (ansi_sequence_character == 'B') {
					seqValue = strtonum(seqGrab, 0, INT_MAX,
							    &errstr);

					if (seqValue)
						row += seqValue;
				} else if (ansi_sequence_character == 'C') {
					seqValue = strtonum(seqGrab, 0, INT_MAX,
							    &errstr);

					if (seqValue)
						column += seqValue;

					if (column >= (int32_t)columns)
						column = columns - 1;
				} else if (ansi_sequence_character == 'D') {
					seqValue = strtonum(seqGrab, 0, INT_MAX,
							    &errstr);

					if (seqValue)
						column -= seqValue;

					if (column < 0)
						column = 0;
				} else if (ansi_sequence_character == 's') {
					saved_column = column;
					saved_row = row;
				} else if (ansi_sequence_character == 'u') {
					column = saved_column;
					row = saved_row;
				} else if (ansi_sequence_character == 'm') {
					seqTok = strtok(seqGrab, ";");

					while (seqTok) {
						seqValue = strtonum(seqTok, 0,
								    INT_MAX, &errstr);

						if (seqValue == 0) {
							bold = false;
							blink = false;
							invert = false;
							foreground = 7;
							background = 0;
						} else if (seqValue == 1) {
							bold = true;
						} else if (seqValue == 5) {
							blink = true;
						} else if (seqValue == 7) {
							invert = true;
						} else if (seqValue >= 30 &&
							   seqValue <= 37) {
							foreground = seqValue - 30;
						} else if (seqValue >= 40 &&
							   seqValue <= 47) {
							background = seqValue - 40;
						}

						seqTok = strtok(NULL, ";");
					}
				}

				free(seqGrab);
				seqGrab = NULL;
				state = STATE_TEXT;
			}
			break;
		}

		if (state == STATE_END)
			break;
	}

	if (seqGrab)
		free(seqGrab);

	ctx->maplen = (grid->max_row + 1) * (grid->max_column + 2) * 50;

	if (ctx->maplen == 0)
		ctx->maplen = 1024;

	ctx->buffer = malloc(ctx->maplen);
	if (!ctx->buffer) {
		ctx->error = ANSILOVE_MEMORY_ERROR;
		terminal_grid_free(grid);
		return -1;
	}

	size_t out_pos = 0;
	struct terminal_cell *prev_cell = NULL;

	for (int32_t r = 0; r <= grid->max_row; r++) {
		for (int32_t c = 0; c <= grid->max_column + 1; c++) {
			if (c <= grid->max_column) {
				if (terminal_emit_cell(&ctx->buffer, &ctx->maplen, &out_pos,
						       &grid->cells[r][c], prev_cell) < 0) {
					ctx->error = ANSILOVE_MEMORY_ERROR;
					terminal_grid_free(grid);
					return -1;
				}

				prev_cell = &grid->cells[r][c];
			} else {
				ctx->buffer[out_pos++] = '\n';
			}
		}
	}

	ctx->length = out_pos;

	terminal_grid_free(grid);

	return 0;
}

uint8_t *
ansilove_terminal_emit(struct ansilove_ctx *ctx, size_t *length)
{
	if (ctx == NULL || length == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return NULL;
	}

	*length = ctx->length;
	return ctx->buffer;
}
