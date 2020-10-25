/*
 * config.h
 * libansilove 1.2.8
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define LF	'\n'
#define CR	'\r'
#define TAB	'\t'
#define SUB	26
#define ESC	27

#define PCB_COLORS	71

static const uint8_t ansi_palette_red[] = {
	0x00, 0xaa, 0x00, 0xaa, 0x00, 0xaa, 0x00, 0xaa,
	0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff
};

static const uint8_t ansi_palette_green[] = {
	0x00, 0x00, 0xaa, 0x55, 0x00, 0x00, 0xaa, 0xaa,
	0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff
};

static const uint8_t ansi_palette_blue[] = {
	0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa,
	0x55, 0x55, 0x55, 0x55, 0xff, 0xff, 0xff, 0xff
};

static const uint8_t workbench_palette_red[] = {
	0xaa, 0x00, 0xff, 0x66, 0x00, 0xff, 0x00, 0xff,
	0xaa, 0x00, 0xff, 0x66, 0x00, 0xff, 0x00, 0xff
};

static const uint8_t workbench_palette_green[] = {
	0xaa, 0x00, 0xff, 0x88, 0x00, 0x00, 0xff, 0xff,
	0xaa, 0x00, 0xff, 0x88, 0x00, 0x00, 0xff, 0xff
};

static const uint8_t workbench_palette_blue[] = {
	0xaa, 0x00, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xff,
	0xaa, 0x00, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xff
};

static const uint8_t vga_palette_red[] = {
	0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa,
	0x55, 0x55, 0x55, 0x55, 0xff, 0xff, 0xff, 0xff
};

static const uint8_t vga_palette_green[] = {
	0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x55, 0xaa,
	0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff
};

static const uint8_t vga_palette_blue[] = {
	0x00, 0xaa, 0x00, 0xaa, 0x00, 0xaa, 0x00, 0xaa,
	0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff
};

/* ADF color mapping array */
static const uint8_t adf_colors[16] = { 0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63 };

static const uint8_t pcb_colors[PCB_COLORS] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x04, 0x02, 0x06, 0x01, 0x05, 0x03, 0x07, 0x08, 0x0c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0e, 0x09, 0x0d, 0x0b, 0x0f
};

#endif /* CONFIG_H */
