//
//  loadfile.c
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

int
ansilove_loadfile(struct ansilove_ctx *ctx, char *input) {
	int fd;
	struct stat st;

		// load input file
		if ((fd = open(input, O_RDONLY)) == -1) {
			// perror("File error");
			return -1;
		}

		// get the file size (bytes)
		if (fstat(fd, &st) == -1) {
			// perror("Can't stat file");
			return -1;
		}

		ctx->length = st.st_size;

		// mmap input file into memory
		ctx->buffer = mmap(NULL, ctx->length, PROT_READ, MAP_PRIVATE, fd, 0);
		if (ctx->buffer == MAP_FAILED) {
			// perror("Memory error");
			return -1;
		}

		// close input file, we don't need it anymore
		close(fd);

		return 0;
}
