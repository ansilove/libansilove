#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"

int main() {
    struct ansilove_ctx ctx;
    struct ansilove_options opts;

    memset(&ctx, 0, sizeof(ctx));
    memset(&opts, 0, sizeof(opts));

    if (ansilove_init(&ctx, &opts) != 0) {
        fprintf(stderr, "init failed\n");
        return 1;
    }

    if (ansilove_loadfile(&ctx, "/home/tom/Downloads/fire-43/AVG-LARA.ANS") != 0) {
        fprintf(stderr, "loadfile failed: %s\n", ansilove_error(&ctx));
        return 1;
    }

    fprintf(stderr, "Loaded %zu bytes\n", ctx.length);

    opts.mode = ANSILOVE_MODE_TERMINAL;
    
    int result = ansilove_terminal(&ctx, &opts);
    fprintf(stderr, "ansilove_terminal returned: %d\n", result);
    if (result != 0) {
        fprintf(stderr, "Error: %s\n", ansilove_error(&ctx));
    }
    
    fprintf(stderr, "ctx.length after: %zu\n", ctx.length);

    ansilove_clean(&ctx);
    return 0;
}
