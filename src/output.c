//
//  output.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "ansilove.h"
#include "output.h"

int output(struct ansilove_ctx *ctx, struct ansilove_options *options, gdImagePtr im_Source) {
	// XXX Error handling
	// XXX The caller must invoke gdFree()
	// XXX Allow combining DOS aspect ratio and Retina

	if (!options->retinaScaleFactor && !options->dos) {
		ctx->png.buffer = gdImagePngPtr(im_Source, &ctx->png.length);
	} else if (options->dos) {
		gdImagePtr im_Resize;
		gdImagePtr im_DOS;

		im_Resize = gdImageCreate(im_Source->sx,
		    im_Source->sy * 27);

		gdImageCopyResized(im_Resize, im_Source, 0, 0, 0, 0,
		    im_Resize->sx, im_Resize->sy, im_Source->sx, im_Source->sy);

		gdImageDestroy(im_Source);

		im_DOS = gdImageCreateTrueColor(im_Resize->sx,
		    im_Resize->sy / 20);

		gdImageCopyResampled(im_DOS, im_Resize, 0, 0, 0, 0,
		    im_DOS->sx, im_DOS->sy, im_Resize->sx, im_Resize->sy);

		gdImageDestroy(im_Resize);

		ctx->png.buffer = gdImagePngPtr(im_DOS, &ctx->png.length);
	} else {
		gdImagePtr im_Retina;

		// make the Retina image retinaScaleFactor as large as im_Source
		im_Retina = gdImageCreate(im_Source->sx * options->retinaScaleFactor,
		    im_Source->sy * options->retinaScaleFactor);

		gdImageCopyResized(im_Retina, im_Source, 0, 0, 0, 0,
		    im_Retina->sx, im_Retina->sy, im_Source->sx, im_Source->sy);

		// create retina output image
		gdImageDestroy(im_Source);
		ctx->png.buffer = gdImagePngPtr(im_Retina, &ctx->png.length);
	}

	return 0;
}
