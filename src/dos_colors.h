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
 * Convert DOS color index (0-15) to closest ANSI 256-color code
 * Pre-calculated best matches from 256-color palette
 */
static inline uint8_t
dos_color_to_ansi256(uint8_t dos_index)
{
	static const uint8_t dos_to_ansi256[16] = {
		16,  /* 0:  Black      #000000 -> ANSI 16  */
		19,  /* 1:  Blue       #0000AA -> ANSI 19  */
		34,  /* 2:  Green      #00AA00 -> ANSI 34  */
		37,  /* 3:  Cyan       #00AAAA -> ANSI 37  */
		124, /* 4:  Red        #AA0000 -> ANSI 124 */
		127, /* 5:  Magenta    #AA00AA -> ANSI 127 */
		136, /* 6:  Brown      #AA5500 -> ANSI 136 */
		248, /* 7:  Light Gray #AAAAAA -> ANSI 248 */
		240, /* 8:  Dark Gray  #555555 -> ANSI 240 */
		105, /* 9:  Light Blue #5555FF -> ANSI 105 */
		120, /* 10: Light Green#55FF55 -> ANSI 120 */
		123, /* 11: Light Cyan #55FFFF -> ANSI 123 */
		210, /* 12: Light Red  #FF5555 -> ANSI 210 */
		213, /* 13: Light Mag. #FF55FF -> ANSI 213 */
		228, /* 14: Yellow     #FFFF55 -> ANSI 228 */
		231, /* 15: White      #FFFFFF -> ANSI 231 */
	};

	if (dos_index >= 16)
		return 7;

	return dos_to_ansi256[dos_index];
}



#endif /* DOS_COLORS_H */
