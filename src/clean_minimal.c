#include <stdlib.h>
#include "ansilove.h"

int
ansilove_clean(struct ansilove_ctx *ctx)
{
	if (ctx == NULL)
		return -1;

	if (ctx->buffer != NULL)
		free(ctx->buffer);

	ctx->maplen = ctx->length = 0;
	return 0;
}
