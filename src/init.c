//
//  init.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include "ansilove.h"

void
ansilove_init(struct ansilove_ctx *ctx, struct ansilove_options *options) {
	// default to 8 if bits option is not specified
	options->bits = 8;

	// default to 160 if columns option is not specified
	options->columns = 160;

	// default to 0 if retinaScaleFactor option is not specified
	options->retinaScaleFactor = 0;
}
