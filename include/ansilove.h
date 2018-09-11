//
//  ansilove.h
//  AnsiLove/C
//
//  Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
//  All rights reserved.
//
//  This source code is licensed under the BSD 2-Clause License.
//  See the LICENSE file for details.
//

#ifndef ansilove_h
#define ansilove_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INVALID_PARAM 1
#define FORMAT_ERROR 2
#define MEMORY_ERROR 3
#define GD_ERROR 4

struct ansilove_png {
	unsigned char *buffer;
	int length;
};

struct ansilove_ctx {
	unsigned char *buffer;
	size_t length;
	struct ansilove_png png;
	int error;
};

struct ansilove_options {
	char *fileName;
	char *retina;
	char *font;
	uint32_t bits;
	uint32_t columns;
	char *mode;
	bool diz;
	bool icecolors;
	uint32_t retinaScaleFactor;
};

void ansilove_init(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_loadfile(struct ansilove_ctx *, char *);

int ansilove_ansi(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_artworx(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_binary(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_icedraw(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_pcboard(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_tundra(struct ansilove_ctx *, struct ansilove_options *);
int ansilove_xbin(struct ansilove_ctx *, struct ansilove_options *);

#endif
