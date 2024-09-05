/*
 * ansilove.h
 * libansilove 1.4.1
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2023 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause license.
 * See LICENSE file for details.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ANSILOVE_H
#define ANSILOVE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ANSILOVE_EXTERN
#define ANSILOVE_EXTERN __attribute__((visibility("default")))
#endif

/* Version number */
#define ANSILOVE_VERSION "1.4.1"

#define ANSILOVE_VERSION_MAJOR		1
#define ANSILOVE_VERSION_MINOR		4
#define ANSILOVE_VERSION_PATCH		1

/* Error codes */
#define ANSILOVE_INVALID_PARAM		1
#define ANSILOVE_FORMAT_ERROR		2
#define ANSILOVE_MEMORY_ERROR		3
#define ANSILOVE_RANGE_ERROR		4
#define ANSILOVE_FILE_READ_ERROR	5
#define ANSILOVE_FILE_WRITE_ERROR	6
#define ANSILOVE_GD_ERROR		7

/* PC Fonts */
#define ANSILOVE_FONT_CP437		1	/* IBM PC 80x25 */
#define ANSILOVE_FONT_CP437_80x50	2	/* IBM PC 80x50 */
#define ANSILOVE_FONT_CP737		3	/* Greek */
#define ANSILOVE_FONT_CP775		4	/* Baltic */
#define ANSILOVE_FONT_CP850		5	/* Latin 1 */
#define ANSILOVE_FONT_CP852		6	/* Latin 2 */
#define ANSILOVE_FONT_CP855		7	/* Cyrillic */
#define ANSILOVE_FONT_CP857		8	/* Turkish */
#define ANSILOVE_FONT_CP860		9	/* Portuguese */
#define ANSILOVE_FONT_CP861		10	/* Icelandic */
#define ANSILOVE_FONT_CP862		11	/* Hebrew */
#define ANSILOVE_FONT_CP863		12	/* French-canadian */
#define ANSILOVE_FONT_CP865		13	/* Nordic */
#define ANSILOVE_FONT_CP866		14	/* Russian */
#define ANSILOVE_FONT_CP869		15	/* Greek */
#define ANSILOVE_FONT_TERMINUS		20
#define ANSILOVE_FONT_SPLEEN		21

/* Amiga fonts */
#define ANSILOVE_FONT_MICROKNIGHT	30
#define ANSILOVE_FONT_MICROKNIGHT_PLUS	31
#define ANSILOVE_FONT_MOSOUL		32
#define ANSILOVE_FONT_POT_NOODLE	33
#define ANSILOVE_FONT_TOPAZ		34
#define ANSILOVE_FONT_TOPAZ_PLUS	35
#define ANSILOVE_FONT_TOPAZ500		36
#define ANSILOVE_FONT_TOPAZ500_PLUS	37

/* Rendering modes */
#define ANSILOVE_MODE_CED		1
#define ANSILOVE_MODE_TRANSPARENT	2
#define ANSILOVE_MODE_WORKBENCH		3

struct ansilove_png {
	uint8_t *buffer;
	int length; /* Match the parameter type of gdImagePngPtr() */
};

struct ansilove_ctx {
	uint8_t *buffer;
	size_t maplen;
	size_t length;
	struct ansilove_png png;
	uint8_t error;
};

struct ansilove_options {
	bool diz;
	bool dos;
	bool icecolors;
	bool truecolor;
	int16_t columns;
	uint8_t font;
	uint8_t bits;
	uint8_t mode;
	uint8_t scale_factor;
};

ANSILOVE_EXTERN int	 ansilove_init(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN char	*ansilove_error(struct ansilove_ctx *);
ANSILOVE_EXTERN int	 ansilove_loadfile(struct ansilove_ctx *, const char *);
ANSILOVE_EXTERN int	 ansilove_savefile(struct ansilove_ctx *, const char *);
ANSILOVE_EXTERN int	 ansilove_clean(struct ansilove_ctx *);

ANSILOVE_EXTERN int	 ansilove_ansi(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN int	 ansilove_artworx(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN int	 ansilove_binary(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN int	 ansilove_icedraw(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN int	 ansilove_pcboard(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN int	 ansilove_tundra(struct ansilove_ctx *, struct ansilove_options *);
ANSILOVE_EXTERN int	 ansilove_xbin(struct ansilove_ctx *, struct ansilove_options *);

#ifdef __cplusplus
}
#endif

#endif /* ANSILOVE_H */
