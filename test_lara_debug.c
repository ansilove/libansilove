#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "ansilove.h"

extern int ansilove_terminal(struct ansilove_ctx *ctx, struct ansilove_options *options);

int main() {
    struct ansilove_ctx ctx;
    struct ansilove_options opts;

    memset(&ctx, 0, sizeof(ctx));
    memset(&opts, 0, sizeof(opts));

    ansilove_init(&ctx, &opts);
    ansilove_loadfile(&ctx, "/home/tom/Downloads/fire-43/AVG-LARA.ANS");

    fprintf(stderr, "Loaded %zu bytes, buffer=%p\n", ctx.length, ctx.buffer);

    opts.mode = ANSILOVE_MODE_TERMINAL;
    opts.columns = 80;
    
    int result = ansilove_terminal(&ctx, &opts);
    fprintf(stderr, "Result: %d, error=%d\n", result, ctx.error);
    
    if (ctx.buffer && ctx.buffer != MAP_FAILED) {
        fprintf(stderr, "Buffer valid: %p, length=%zu\n", ctx.buffer, ctx.length);
    }

    ansilove_clean(&ctx);
    return 0;
}
