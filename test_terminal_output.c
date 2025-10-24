#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.ans>\n", argv[0]);
        return 1;
    }

    struct ansilove_ctx ctx;
    struct ansilove_options opts;

    memset(&ctx, 0, sizeof(ctx));
    memset(&opts, 0, sizeof(opts));

    if (ansilove_init(&ctx, &opts) != 0) {
        fprintf(stderr, "Init failed\n");
        return 1;
    }

    if (ansilove_loadfile(&ctx, argv[1]) != 0) {
        fprintf(stderr, "Load failed\n");
        return 1;
    }

    if (ansilove_terminal(&ctx, &opts) != 0) {
        fprintf(stderr, "Terminal conversion failed\n");
        return 1;
    }

    size_t output_len;
    uint8_t *output = ansilove_terminal_emit(&ctx, &output_len);
    
    if (output && output_len > 0) {
        fwrite(output, 1, output_len, stdout);
    }
    
    ansilove_clean(&ctx);
    return 0;
}
