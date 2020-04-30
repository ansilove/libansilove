/*
 * output.c
 * libansilove 1.2.1
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <gd.h>
#include "ansilove.h"
#include "output.h"

int
output(struct ansilove_ctx *ctx, struct ansilove_options *options,
    gdImagePtr im_Source)
{
	/* XXX Error handling */
	/* XXX The caller must invoke gdFree() */

	/* Handle DOS aspect ratio */
	if (options->dos) {
		gdImagePtr im_DOS = gdImageCreateTrueColor(im_Source->sx,
		    im_Source->sy * 1.35);

		if (!im_DOS) {
			ctx->error = ANSILOVE_GD_ERROR;
			return -1;
		}

		gdImageCopyResampled(im_DOS, im_Source, 0, 0, 0, 0,
		    im_DOS->sx, im_DOS->sy, im_Source->sx, im_Source->sy);

		gdImageDestroy(im_Source);
		im_Source = im_DOS;
	}

	/* Handle resizing */
	if (options->scale_factor) {
		gdImagePtr im_Retina;

		if gdImageTrueColor(im_Source) {
			im_Retina = gdImageCreateTrueColor(im_Source->sx *
			     options->scale_factor, im_Source->sy * options->scale_factor);
		} else {
			im_Retina = gdImageCreate(im_Source->sx *
			     options->scale_factor, im_Source->sy * options->scale_factor);
		}

		if (!im_Retina) {
			ctx->error = ANSILOVE_GD_ERROR;
			return -1;
		}

		gdImageCopyResized(im_Retina, im_Source, 0, 0, 0, 0,
		    im_Retina->sx, im_Retina->sy, im_Source->sx, im_Source->sy);

		gdImageDestroy(im_Source);
		im_Source = im_Retina;
	}

	/* Handle transparency */
	if (options->mode == ANSILOVE_MODE_TRANSPARENT)
		gdImageColorTransparent(im_Source, 0);

	ctx->png.buffer = gdImagePngPtr(im_Source, &ctx->png.length);

	gdImageDestroy(im_Source);

	return 0;
}
