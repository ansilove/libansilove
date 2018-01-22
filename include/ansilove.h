//
//  ansilove.h
//  AnsiLove/C
//
//  Copyright (C) 2011-2017 Stefan Vogt, Brian Cassidy, and Frederic Cambus.
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
	uint32_t columns;
};

struct output {
	char *fileName;
	char *retina;
	char *font;
	uint32_t bits;
	char *mode;
	bool icecolors;
	uint32_t retinaScaleFactor;
};

void ansi(struct input *, struct output *);
void artworx(struct input *, struct output *);
void binary(struct input *, struct output *);
void icedraw(struct input *, struct output *);
void pcboard(struct input *, struct output *);
void tundra(struct input *, struct output *);
void xbin(struct input *, struct output *);

#endif
