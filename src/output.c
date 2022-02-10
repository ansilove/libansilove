/*
 * output.c
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
#include "ansilove.h"
#include "output.h"

int
output(struct ansilove_ctx *ctx, struct ansilove_options *options,
    gdImagePtr source)
{
	/* Handle DOS aspect ratio */
	if (options->dos) {
		gdImagePtr dos = gdImageCreateTrueColor(source->sx,
		    source->sy * 1.35);

		if (!dos) {
			ctx->error = ANSILOVE_GD_ERROR;
			return -1;
		}

		gdImageCopyResampled(dos, source, 0, 0, 0, 0,
		    dos->sx, dos->sy, source->sx, source->sy);

		gdImageDestroy(source);
		source = dos;
	}

	/* Handle resizing */
	if (options->scale_factor) {
		if (options->scale_factor < 2 || options->scale_factor > 8) {
			ctx->error = ANSILOVE_RANGE_ERROR;
			return -1;
		}

		uint32_t width, height;
		gdImagePtr retina;

		width = source->sx * options->scale_factor;
		height = source->sy * options->scale_factor;

		retina = gdImageTrueColor(source) ?
			gdImageCreateTrueColor(width, height) :
			gdImageCreate(width, height);

		if (!retina) {
			ctx->error = ANSILOVE_GD_ERROR;
			return -1;
		}

		gdImageCopyResized(retina, source, 0, 0, 0, 0,
		    retina->sx, retina->sy, source->sx, source->sy);

		gdImageDestroy(source);
		source = retina;
	}

	/* Handle transparency */
	if (options->mode == ANSILOVE_MODE_TRANSPARENT)
		gdImageColorTransparent(source, 0);

	ctx->png.buffer = gdImagePngPtr(source, &ctx->png.length);

	gdImageDestroy(source);

	return 0;
}
