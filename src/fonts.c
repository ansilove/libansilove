/*
 * fonts.c
 * libansilove 1.1.8
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include "ansilove.h"
#include "fonts.h"

#include "fonts/font_pc_80x25.h"
#include "fonts/font_pc_80x50.h"
#include "fonts/font_pc_baltic.h"
#include "fonts/font_pc_cyrillic.h"
#include "fonts/font_pc_french_canadian.h"
#include "fonts/font_pc_greek.h"
#include "fonts/font_pc_greek869.h"
#include "fonts/font_pc_hebrew.h"
#include "fonts/font_pc_icelandic.h"
#include "fonts/font_pc_latin1.h"
#include "fonts/font_pc_latin2.h"
#include "fonts/font_pc_nordic.h"
#include "fonts/font_pc_portuguese.h"
#include "fonts/font_pc_russian.h"
#include "fonts/font_pc_terminus.h"
#include "fonts/font_pc_turkish.h"
#include "fonts/font_amiga_microknight.h"
#include "fonts/font_amiga_microknight_plus.h"
#include "fonts/font_amiga_mosoul.h"
#include "fonts/font_amiga_pot_noodle.h"
#include "fonts/font_amiga_topaz_1200.h"
#include "fonts/font_amiga_topaz_1200_plus.h"
#include "fonts/font_amiga_topaz_500.h"
#include "fonts/font_amiga_topaz_500_plus.h"

void
select_font(struct fontStruct *fontData, int font)
{
	switch (font) {
	case ANSILOVE_FONT_CP437_80x50:
		fontData->font_data = font_pc_80x50;
		fontData->width = 9;
		fontData->height = 8;
		break;
	case ANSILOVE_FONT_CP737:
		fontData->font_data = font_pc_greek;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP775:
		fontData->font_data = font_pc_baltic;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP850:
		fontData->font_data = font_pc_latin1;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP852:
		fontData->font_data = font_pc_latin2;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP855:
		fontData->font_data = font_pc_cyrillic;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP857:
		fontData->font_data = font_pc_turkish;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP860:
		fontData->font_data = font_pc_portuguese;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP861:
		fontData->font_data = font_pc_icelandic;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP862:
		fontData->font_data = font_pc_hebrew;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP863:
		fontData->font_data = font_pc_french_canadian;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP865:
		fontData->font_data = font_pc_nordic;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP866:
		fontData->font_data = font_pc_russian;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_CP869:
		fontData->font_data = font_pc_greek_869;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_TERMINUS:
		fontData->font_data = font_pc_terminus;
		fontData->width = 9;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_MICROKNIGHT:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_microknight;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_MICROKNIGHT_PLUS:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_microknight_plus;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_MOSOUL:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_mosoul;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_POT_NOODLE:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_pot_noodle;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_TOPAZ:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_topaz_1200;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_TOPAZ_PLUS:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_topaz_1200_plus;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_TOPAZ500:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_topaz_500;
		fontData->width = 8;
		fontData->height = 16;
		break;
	case ANSILOVE_FONT_TOPAZ500_PLUS:
		fontData->isAmigaFont = true;
		fontData->font_data = font_amiga_topaz_500_plus;
		fontData->width = 8;
		fontData->height = 16;
		break;
	default:
		/* in all other cases use the standard DOS font */
		fontData->font_data = font_pc_80x25;
		fontData->width = 9;
		fontData->height = 16;
		break;
	}
}
