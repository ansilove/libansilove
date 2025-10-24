#include <stdio.h>
#include <stdint.h>
#include "ansilove.h"

int main() {
    struct ansilove_ctx ctx;
    struct ansilove_options opts = { 0 };
    
    if (ansilove_init(&ctx, &opts) != 0)
        return 1;
    
    if (ansilove_loadfile(&ctx, "/home/tom/Downloads/fire-39/H4-2017.ANS") != 0)
        return 1;
    
    printf("File: %zu bytes\n", ctx.length);
    
    if (ansilove_terminal(&ctx, &opts) != 0) {
        printf("Parser error\n");
        return 1;
    }
    
    printf("Parser finished\n");
    
    size_t output_len = 0;
    uint8_t *output = ansilove_terminal_emit(&ctx, &output_len);
    
    printf("Output: %zu bytes\n", output_len);
    
    int visible_on_first = 0;
    int i = 0;
    while (i < output_len && output[i] != '\n') {
        if (output[i] == '\x1b') {
            while (i < output_len && output[i] != 'm') i++;
            i++;
        } else {
            visible_on_first++;
            i++;
        }
    }
    printf("First line visible chars: %d (should be 80)\n", visible_on_first);
    
    ansilove_clean(&ctx);
    return 0;
}
