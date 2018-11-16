/*
 * clean.c
 * libansilove 1.0.0
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <stddef.h>
#include "ansilove.h"
#include "gd.h"

int
ansilove_clean(struct ansilove_ctx *ctx) {
	if (ctx == NULL)
		return -1;

	if (ctx->png.buffer != NULL)
		gdFree(ctx->png.buffer);
	ctx->png.length = 0;

	return 0;
}
