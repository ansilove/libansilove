#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <ansi-file>... [columns]\n", argv[0]);
        return 1;
    }

    int columns = 0;
    int file_count = argc - 1;
    
    if (argc >= 3) {
        char *endptr;
        long val = strtol(argv[argc - 1], &endptr, 10);
        if (*endptr == '\0' && val > 0 && val < 10000) {
            columns = val;
            file_count = argc - 2;
        }
    }

    for (int i = 1; i <= file_count; i++) {
        struct ansilove_ctx ctx;
        struct ansilove_options opts;

        memset(&ctx, 0, sizeof(ctx));
        memset(&opts, 0, sizeof(opts));

        if (ansilove_init(&ctx, &opts) != 0) {
            fprintf(stderr, "Init failed: %s\n", argv[i]);
            continue;
        }

        if (ansilove_loadfile(&ctx, argv[i]) != 0) {
            fprintf(stderr, "Load failed: %s\n", argv[i]);
            ansilove_clean(&ctx);
            continue;
        }

        opts.mode = ANSILOVE_MODE_TERMINAL;
        if (columns > 0) {
            opts.columns = columns;
        }
        
        if (ansilove_terminal(&ctx, &opts) != 0) {
            fprintf(stderr, "Terminal conversion failed: %s\n", argv[i]);
            ansilove_clean(&ctx);
            continue;
        }

        size_t output_len;
        uint8_t *output = ansilove_terminal_emit(&ctx, &output_len);
        
        if (output && output_len > 0) {
            fwrite(output, 1, output_len, stdout);
        }

        ansilove_clean(&ctx);
    }
    
    return 0;
}
