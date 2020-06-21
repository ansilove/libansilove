/*
 * fonts.h
 * libansilove 1.2.4
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#ifndef FONTS_H
#define FONTS_H

#include <stdbool.h>
#include <stdint.h>

struct fontStruct {
	const uint8_t *font_data;
	uint32_t width;
	uint32_t height;
	bool isAmigaFont;
};

void select_font(struct fontStruct *, int);

extern const uint8_t font_pc_80x25[4096];
extern const uint8_t font_pc_80x50[2048];
extern const uint8_t font_pc_baltic[4096];
extern const uint8_t font_pc_cyrillic[4096];
extern const uint8_t font_pc_french_canadian[4096];
extern const uint8_t font_pc_greek[4096];
extern const uint8_t font_pc_greek_869[4096];
extern const uint8_t font_pc_hebrew[4096];
extern const uint8_t font_pc_icelandic[4096];
extern const uint8_t font_pc_latin1[4096];
extern const uint8_t font_pc_latin2[4096];
extern const uint8_t font_pc_nordic[4096];
extern const uint8_t font_pc_portuguese[4096];
extern const uint8_t font_pc_russian[4096];
extern const uint8_t font_pc_terminus[4096];
extern const uint8_t font_pc_turkish[4096];

extern const uint8_t font_amiga_microknight[4096];
extern const uint8_t font_amiga_microknight_plus[4096];
extern const uint8_t font_amiga_mosoul[4096];
extern const uint8_t font_amiga_pot_noodle[4096];
extern const uint8_t font_amiga_topaz_1200[4096];
extern const uint8_t font_amiga_topaz_1200_plus[4096];
extern const uint8_t font_amiga_topaz_500[4096];
extern const uint8_t font_amiga_topaz_500_plus[4096];

#endif /* FONTS_H */
