// SPDX-FileCopyrightText: 2024 Martin J. Fiedler <keyj@emphy.de>
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "strndup.h"

char *strndup(const char *str, size_t size) {
    char *buffer;
    size_t srclen = strlen(str);
    if (srclen < size) { size = srclen; }
    buffer = (char*) malloc(size + 1);
    if (buffer) {
        strncpy(buffer, str, size);
    }
    buffer[size] = '\0';
    return buffer;
}
