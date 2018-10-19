//
//  error.c
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "ansilove.h"

char*
ansilove_error(struct ansilove_ctx *ctx) {
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
		case ANSILOVE_FILE_ERROR:
			return "File error.";
			break;
		case ANSILOVE_GD_ERROR:
			return "GD error.";
			break;
		default:
			return NULL;
	}
}
