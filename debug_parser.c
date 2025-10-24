#include <stdio.h>
#include <stdint.h>
#include "ansilove.h"

int main() {
    struct ansilove_ctx ctx;
    struct ansilove_options opts = { 0 };
    
    if (ansilove_init(&ctx, &opts) != 0)
        return 1;
    
    if (ansilove_loadfile(&ctx, "/home/tom/Downloads/fire-39/CAL24-01.ANS") != 0)
        return 1;
    
    printf("File loaded: %zu bytes\n", ctx.length);
    
    if (ansilove_terminal(&ctx, &opts) != 0) {
        printf("Parser error\n");
        return 1;
    }
    
    printf("Parser finished\n");
    
    size_t output_len = 0;
    uint8_t *output = ansilove_terminal_emit(&ctx, &output_len);
    
    printf("Output: %zu bytes\n", output_len);
    
    int newline_count = 0;
    for (size_t i = 0; i < output_len; i++) {
        if (output[i] == '\n') newline_count++;
    }
    printf("Newlines in output: %d\n", newline_count);
    
    ansilove_clean(&ctx);
    return 0;
}
