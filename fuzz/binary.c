#include <ansilove.h>

int
LLVMFuzzerTestOneInput(uint8_t *data, size_t size)
{
	struct ansilove_ctx ctx;
	struct ansilove_options options;

	ansilove_init(&ctx, &options);

	ctx.buffer = data;
	ctx.length = size;

	ansilove_binary(&ctx, &options);

	ansilove_clean(&ctx);
	return 0;
}
