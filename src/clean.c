/*
 * clean.c
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

#include <sys/mman.h>

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

	if (ctx->buffer != MAP_FAILED)
		(void)munmap(ctx->buffer, ctx->maplen);

	ctx->maplen = ctx->length = 0;
	ctx->png.length = 0;

	return 0;
}
