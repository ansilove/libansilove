#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansilove.h"

int
main(int argc, char *argv[])
{
	struct ansilove_ctx ctx;
	struct ansilove_options opts;
	uint8_t *output;
	size_t output_len;
	int result;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <ansi-file> [output-file]\n", argv[0]);
		return 1;
	}

	memset(&ctx, 0, sizeof(ctx));
	memset(&opts, 0, sizeof(opts));

	result = ansilove_init(&ctx, &opts);
	if (result != 0) {
		fprintf(stderr, "ansilove_init failed: %s\n", ansilove_error(&ctx));
		return 1;
	}

	result = ansilove_loadfile(&ctx, argv[1]);
	if (result != 0) {
		fprintf(stderr, "ansilove_loadfile failed: %s\n", ansilove_error(&ctx));
		ansilove_clean(&ctx);
		return 1;
	}

	opts.mode = ANSILOVE_MODE_TERMINAL;
	opts.columns = 80;

	result = ansilove_terminal(&ctx, &opts);
	if (result != 0) {
		fprintf(stderr, "ansilove_terminal failed: %s\n", ansilove_error(&ctx));
		ansilove_clean(&ctx);
		return 1;
	}

	output = ansilove_terminal_emit(&ctx, &output_len);
	if (!output) {
		fprintf(stderr, "ansilove_terminal_emit failed\n");
		ansilove_clean(&ctx);
		return 1;
	}

	if (argc >= 3) {
		FILE *fp = fopen(argv[2], "wb");
		if (!fp) {
			fprintf(stderr, "Cannot open output file: %s\n", argv[2]);
			ansilove_clean(&ctx);
			return 1;
		}

		if (fwrite(output, 1, output_len, fp) != output_len) {
			fprintf(stderr, "Write failed\n");
			fclose(fp);
			ansilove_clean(&ctx);
			return 1;
		}

		fclose(fp);
		printf("Output written to %s (%zu bytes)\n", argv[2], output_len);
	} else {
		if (fwrite(output, 1, output_len, stdout) != output_len) {
			fprintf(stderr, "Write to stdout failed\n");
			ansilove_clean(&ctx);
			return 1;
		}
	}

	ansilove_clean(&ctx);
	return 0;
}
