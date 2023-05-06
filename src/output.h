/*
 * output.h
 * libansilove 1.3.1
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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <gd.h>

struct ansilove_ctx;
struct ansilove_options;

int output(struct ansilove_ctx *, struct ansilove_options *, gdImagePtr);

#endif /* OUTPUT_H */
