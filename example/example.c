#include <stdio.h>
#include <errno.h>
#include <ansilove.h>

int main() {
	struct ansilove_ctx* ctx = malloc(sizeof *ctx);
	struct ansilove_options* options = malloc(sizeof *options);
	
	if (ctx && options) {

		ansilove_init(ctx, options);

		ansilove_loadfile(ctx, "example.c");

		ansilove_ansi(ctx, options);

		ansilove_savefile(ctx, "example.png");

		ansilove_clean(ctx);

		free(ctx);
		free(options);

	}

	else {
		printf("Malloc function is failed...\n");
		return -ENOMEM;
	}
	
	return 0;
}
