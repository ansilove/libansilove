/*
 * dos_colors.h
 * libansilove
 *
 * CGA/EGA color palette and ANSI 256-color conversion
 */

#ifndef DOS_COLORS_H
#define DOS_COLORS_H

#include <stdint.h>

struct rgb_color {
	uint8_t r, g, b;
};

/* CGA/EGA 16-color palette (DOS standard) */
static const struct rgb_color dos_palette[16] = {
	/* 0: Black */       {0x00, 0x00, 0x00},
	/* 1: Blue */        {0x00, 0x00, 0xAA},
	/* 2: Green */       {0x00, 0xAA, 0x00},
	/* 3: Cyan */        {0x00, 0xAA, 0xAA},
	/* 4: Red */         {0xAA, 0x00, 0x00},
	/* 5: Magenta */     {0xAA, 0x00, 0xAA},
	/* 6: Brown/Yellow */  {0xAA, 0x55, 0x00},
	/* 7: Light Gray */  {0xAA, 0xAA, 0xAA},
	/* 8: Dark Gray */   {0x55, 0x55, 0x55},
	/* 9: Light Blue */  {0x55, 0x55, 0xFF},
	/* 10: Light Green */{0x55, 0xFF, 0x55},
	/* 11: Light Cyan */{0x55, 0xFF, 0xFF},
	/* 12: Light Red */  {0xFF, 0x55, 0x55},
	/* 13: Light Magenta */ {0xFF, 0x55, 0xFF},
	/* 14: Yellow */     {0xFF, 0xFF, 0x55},
	/* 15: White */      {0xFF, 0xFF, 0xFF},
};

/*
 * Initialize DOS color palette lookup (for consistency)
 */
static inline void
dos_palette_init(uint32_t colors[16])
{
	for (int i = 0; i < 16; i++) {
		colors[i] = i;
	}
}

/*
 * RGB to ANSI 256 conversion
 * Uses 6x6x6 color cube for values in range 0-255
 */
static inline uint8_t
rgb_to_ansi256(uint8_t r, uint8_t g, uint8_t b)
{
	/* Grayscale check: if R == G == B, use grayscale range (232-255) */
	if (r == g && g == b) {
		if (r < 48)
			return 16; /* Black */
		else if (r < 100)
			return 59; /* Dark gray */
		else if (r < 155)
			return 102; /* Medium gray */
		else if (r < 205)
			return 188; /* Light gray */
		else
			return 231; /* White */
	}

	/* Map to 6x6x6 color cube (16-231) */
	uint8_t r6 = (r < 48) ? 0 : ((r < 115) ? 1 : ((r < 155) ? 2 :
		      ((r < 195) ? 3 : ((r < 235) ? 4 : 5))));
	uint8_t g6 = (g < 48) ? 0 : ((g < 115) ? 1 : ((g < 155) ? 2 :
		      ((g < 195) ? 3 : ((g < 235) ? 4 : 5))));
	uint8_t b6 = (b < 48) ? 0 : ((b < 115) ? 1 : ((b < 155) ? 2 :
		      ((b < 195) ? 3 : ((b < 235) ? 4 : 5))));

	return 16 + (36 * r6) + (6 * g6) + b6;
}

/*
 * Convert DOS color index (0-15) to ANSI 256-color code
 * Uses RGB conversion for accurate color matching
 */
static inline uint8_t
dos_color_to_ansi256(uint8_t dos_index)
{
	if (dos_index >= 16)
		return 7;

	const struct rgb_color *rgb = &dos_palette[dos_index];
	return rgb_to_ansi256(rgb->r, rgb->g, rgb->b);
}

#endif /* DOS_COLORS_H */
