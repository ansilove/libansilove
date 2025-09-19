/*
 * clean.c
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

#ifndef ANSILOVE_WASM
#include <sys/mman.h>
#endif

#include <stddef.h>
#include "ansilove.h"
#include "gd.h"

int
ansilove_clean(struct ansilove_ctx *ctx)
{
	if (ctx == NULL)
		return -1;

	if (ctx->png.buffer != NULL)
		gdFree(ctx->png.buffer);

#ifndef ANSILOVE_WASM
	if (ctx->buffer != MAP_FAILED && ctx->maplen)
		(void)munmap(ctx->buffer, ctx->maplen);
	ctx->buffer = MAP_FAILED;
#else
	ctx->buffer = NULL;
#endif

	ctx->maplen = ctx->length = 0;
	ctx->png.length = 0;
	ctx->png.buffer = NULL;

	return 0;
}
