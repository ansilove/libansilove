/*
 * drawchar.h
 * libansilove 1.1.7
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2019 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#ifndef DRAWCHAR_H
#define DRAWCHAR_H

#include <gd.h>
#include <sys/types.h>

void drawchar(gdImagePtr, const uint8_t *, uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t, uint32_t, uint8_t);

#endif /* DRAWCHAR_H */
