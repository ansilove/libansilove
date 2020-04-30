/*
 * init.c
 * libansilove 1.2.2
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <sys/mman.h>

#include <string.h>
#include "ansilove.h"

int
ansilove_init(struct ansilove_ctx *ctx, struct ansilove_options *options)
{
	if (ctx == NULL || options == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	memset(ctx, 0, sizeof(*ctx));
	memset(options, 0, sizeof(*options));

	ctx->buffer = MAP_FAILED;

	/* default to 8 if bits option is not specified */
	options->bits = 8;

	return 0;
}
