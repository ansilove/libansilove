#include <ansilove.h>

int main() {
	struct ansilove_ctx ctx;
	struct ansilove_options options;

	ansilove_init(&ctx, &options);

	ansilove_loadfile(&ctx, "example.c");

	ansilove_ansi(&ctx, &options);

	ansilove_savefile(&ctx, "example.png");

	ansilove_clean(&ctx);
}
