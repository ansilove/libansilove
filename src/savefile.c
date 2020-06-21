/*
 * savefile.c
 * libansilove 1.2.4
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <stdio.h>
#include "ansilove.h"

int
ansilove_savefile(struct ansilove_ctx *ctx, char *output)
{
	size_t rw;

	if (ctx == NULL || output == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	FILE *file = fopen(output, "wb");

	if (!file)
		goto err;

	rw = fwrite(ctx->png.buffer, 1, ctx->png.length, file);
	fclose(file);

	if (rw != (size_t)ctx->png.length)
		goto err;

	return 0;

err:
	ctx->error = ANSILOVE_FILE_WRITE_ERROR;
	return -1;
}
