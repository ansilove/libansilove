/*
 * drawchar.h
 * libansilove 1.2.4
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2020 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#ifndef DRAWCHAR_H
#define DRAWCHAR_H

#include <stdint.h>
#include <gd.h>

void drawchar(gdImagePtr, const uint8_t *, uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t, uint32_t, uint8_t);

#endif /* DRAWCHAR_H */
