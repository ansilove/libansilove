/*
 * xbin.c
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

#include <gd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"
#include "config.h"
#include "drawchar.h"
#include "fonts.h"
#include "output.h"

#define XBIN_HEADER_LENGTH 11 /* 4 + 1 + 2 + 2 + 1 + 1 */
#define XBIN_PALETTE_LENGTH 48

int
ansilove_xbin(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	const uint8_t *font_data;
	uint8_t *font_data_xbin = NULL;
	uint8_t *high_font_data_xbin = NULL;
	uint32_t width, height;
	uint32_t colors[16];
	uint32_t offset = XBIN_HEADER_LENGTH;
	uint32_t column = 0, row = 0, foreground, background;
	int32_t character, attribute;

	gdImagePtr canvas;

	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	if (ctx->length < XBIN_HEADER_LENGTH) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	if (strncmp((char *)ctx->buffer, "XBIN\x1a", 5) != 0) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	uint32_t xbin_width = (ctx->buffer[6] << 8) + ctx->buffer[5];
	uint32_t xbin_height = (ctx->buffer[8] << 8) + ctx->buffer[7];
	uint32_t xbin_fontsize = ctx->buffer[9];
	uint32_t xbin_flags = ctx->buffer[10];

	if (xbin_fontsize == 0) {
		xbin_fontsize = 16;
	}

	if (xbin_fontsize > 32) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	if (xbin_width < 1 || xbin_width > 4096) {
		ctx->error = ANSILOVE_RANGE_ERROR;
		return -1;
	}

	width = 8 * xbin_width;
	height = xbin_fontsize * xbin_height;

	if (!width || !height) {
		ctx->error = ANSILOVE_FORMAT_ERROR;
		return -1;
	}

	canvas = gdImageCreate(width, height);

	if (!canvas) {
		ctx->error = ANSILOVE_GD_ERROR;
		return -1;
	}

	/* palette */
	if ((xbin_flags & 1) == 1) {
		size_t index, loop;

		if (offset + XBIN_PALETTE_LENGTH > ctx->length) {
			ctx->error = ANSILOVE_FORMAT_ERROR;
			return -1;
		}

		for (loop = 0; loop < 16; loop++) {
			index = (loop * 3) + offset;

			colors[loop] = gdImageColorAllocate(canvas,
			    ctx->buffer[index] << 2 | ctx->buffer[index] >> 4,
			    ctx->buffer[index + 1] << 2 | ctx->buffer[index + 1] >> 4,
			    ctx->buffer[index + 2] << 2 | ctx->buffer[index + 2] >> 4);
		}

		offset += XBIN_PALETTE_LENGTH;
	} else {
		for (size_t i = 0; i < 16; i++)
			colors[i] = gdImageColorAllocate(canvas,
			    vga_palette_red[i], vga_palette_green[i],
			    vga_palette_blue[i]);
	}

	/* font */
	if ((xbin_flags & 2) == 2) {
		size_t fontsz = xbin_fontsize * 256;

		if (offset + fontsz > ctx->length) {
			ctx->error = ANSILOVE_FORMAT_ERROR;
			return -1;
		}

		/* allocate memory to contain the XBin font */
		font_data_xbin = (uint8_t *)malloc(fontsz);
		if (font_data_xbin == NULL) {
			ctx->error = ANSILOVE_MEMORY_ERROR;
			return -1;
		}

		memcpy(font_data_xbin, ctx->buffer+offset, fontsz);

		font_data = font_data_xbin;

		offset += fontsz;

                /* 512 char font */
		if (xbin_flags & 0x10) {
			if (offset + fontsz > ctx->length) {
				ctx->error = ANSILOVE_FORMAT_ERROR;
				free(font_data_xbin);
				return -1;
			}

			/* allocate memory to contain the XBin font */
			high_font_data_xbin = (uint8_t *)malloc(fontsz);
			if (high_font_data_xbin == NULL) {
				ctx->error = ANSILOVE_MEMORY_ERROR;
				free(font_data_xbin);
				return -1;
			}

			memcpy(high_font_data_xbin, ctx->buffer+offset, fontsz);

			offset += fontsz;
                }

	} else {
		/* using default 80x25 font */
		font_data = font_pc_80x25;
		xbin_fontsize = 16;
	}

	/* read compressed xbin */
	if ((xbin_flags & 4) == 4) {
		while (offset + 1 < ctx->length && row != xbin_height) {
			uint32_t ctype = ctx->buffer[offset] & 0xC0;
			uint32_t counter = (ctx->buffer[offset] & 0x3F) + 1;

			character = -1;
			attribute = -1;

			offset++;
			while (counter--) {
				/* none */
				if (ctype == 0) {
					if (offset + 1 < ctx->length) {
						character = ctx->buffer[offset];
						attribute = ctx->buffer[offset + 1];
						offset += 2;
					} else {
						ctx->error = ANSILOVE_FORMAT_ERROR;
						free(font_data_xbin);
						free(high_font_data_xbin);
						return -1;
					}
				}
				/* char */
				else if (ctype == 0x40) {
					if (character == -1) {
						character = ctx->buffer[offset];
						offset++;
					}
					attribute = ctx->buffer[offset];
					offset++;
				}
				/* attr */
				else if (ctype == 0x80) {
					if (attribute == -1) {
						attribute = ctx->buffer[offset];
						offset++;
					}
					character = ctx->buffer[offset];
					offset++;
				}
				/* both */
				else {
					if (character == -1) {
						character = ctx->buffer[offset];
						offset++;
					}
					if (attribute == -1) {
						attribute = ctx->buffer[offset];
						offset++;
					}
				}

				background = (attribute & 240) >> 4;
				foreground = attribute & 15;

				if (xbin_flags & 0x10 && attribute & 8) {
					drawchar(canvas, high_font_data_xbin, 8, xbin_fontsize,
					    column, row, colors[background],
					    colors[foreground], character);
				}
				else {
					drawchar(canvas, font_data, 8, xbin_fontsize,
					    column, row, colors[background],
					    colors[foreground], character);
				}

				column++;

				if (column == xbin_width) {
					column = 0;
					row++;
				}
			}
		}
	} else {
		/* read uncompressed xbin */
		while (offset + 1 < ctx->length && row != xbin_height) {
			if (column == xbin_width) {
				column = 0;
				row++;
			}

			character = ctx->buffer[offset];
			attribute = ctx->buffer[offset+1];

			background = (attribute & 240) >> 4;
			foreground = attribute & 15;

			if (xbin_flags & 0x10 && attribute & 8) {
				drawchar(canvas, high_font_data_xbin, 8, xbin_fontsize,
				    column, row, colors[background],
				    colors[foreground], character);
			}
			else {
				drawchar(canvas, font_data, 8, xbin_fontsize,
				    column, row, colors[background],
				    colors[foreground], character);
			}

			column++;
			offset += 2;
		}
	}

	/* create output file */
	if (output(ctx, options, canvas) != 0) {
		free(font_data_xbin);
		free(high_font_data_xbin);
		font_data = NULL;
		return -1;
	}

	free(font_data_xbin);
	free(high_font_data_xbin);

	return 0;
}
