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

struct input {
	unsigned char *data;
	size_t size;
	char *fext;
};

struct output {
	char *fileName;
	char *retina;
	char *font;
	uint32_t bits;
	uint32_t columns;
	char *mode;
	bool icecolors;
	uint32_t retinaScaleFactor;
};

int ansilove_ansi(struct input *, struct output *);
int ansilove_artworx(struct input *, struct output *);
int ansilove_binary(struct input *, struct output *);
int ansilove_icedraw(struct input *, struct output *);
int ansilove_pcboard(struct input *, struct output *);
int ansilove_tundra(struct input *, struct output *);
int ansilove_xbin(struct input *, struct output *);

#endif
