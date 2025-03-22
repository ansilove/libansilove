/*
 * error.c
 * libansilove 1.4.1
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

#include <stddef.h>
#include "ansilove.h"

char *
ansilove_error(struct ansilove_ctx *ctx)
{
	if (ctx == NULL)
		return NULL;

	switch (ctx->error) {
	case ANSILOVE_INVALID_PARAM:
		return "Invalid parameter.";
		break;
	case ANSILOVE_FORMAT_ERROR:
		return "File format error.";
		break;
	case ANSILOVE_MEMORY_ERROR:
		return "Memory allocation error.";
		break;
	case ANSILOVE_RANGE_ERROR:
		return "Value out of allowed range.";
		break;
	case ANSILOVE_FILE_READ_ERROR:
		return "Error reading file.";
		break;
	case ANSILOVE_FILE_WRITE_ERROR:
		return "Error writing file.";
		break;
	case ANSILOVE_GD_ERROR:
		return "GD library error.";
		break;
	default:
		return NULL;
	}
}
