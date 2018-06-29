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

int ansi(struct input *, struct output *);
int artworx(struct input *, struct output *);
int binary(struct input *, struct output *);
int icedraw(struct input *, struct output *);
int pcboard(struct input *, struct output *);
int tundra(struct input *, struct output *);
int xbin(struct input *, struct output *);

#endif
