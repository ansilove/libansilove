/*
 * savefile.c
 * libansilove 1.2.1
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
	if (ctx == NULL || output == NULL) {
		if (ctx)
			ctx->error = ANSILOVE_INVALID_PARAM;

		return -1;
	}

	FILE *file = fopen(output, "wb");

	if (file) {
		fwrite(ctx->png.buffer, ctx->png.length, 1, file);
		fclose(file);
	} else {
		ctx->error = ANSILOVE_FILE_WRITE_ERROR;
		return -1;
	}

	return 0;
}
