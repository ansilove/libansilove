/*
 * output.h
 * libansilove 1.0.0
 * https://www.ansilove.org
 *
 * Copyright (c) 2011-2018 Stefan Vogt, Brian Cassidy, and Frederic Cambus
 * All rights reserved.
 *
 * libansilove is licensed under the BSD 2-Clause License.
 * See LICENSE file for details.
 */

#include <gd.h>

struct ansilove_ctx;
struct ansilove_options;

#ifndef OUTPUT_H
#define OUTPUT_H

// prototypes
int output(struct ansilove_ctx *, struct ansilove_options *, gdImagePtr);

#endif /* OUTPUT_H */
