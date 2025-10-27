#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ansilove.h>

/*
 * Example: Terminal Mode Output
 * 
 * This example demonstrates how to use libansilove to output
 * ANSI art directly to terminal as UTF-8+ANSI SGR codes.
 * 
 * Unlike the default PNG mode, terminal mode:
 * - Generates text output instead of images
 * - Uses UTF-8 encoding for characters
 * - Preserves colors with ANSI SGR codes
 * - Supports box-drawing characters
 * - Small output size suitable for pipes/redirection
 */

int
main(int argc, char *argv[])
{
	struct ansilove_ctx ctx;
	struct ansilove_options opts;
	uint8_t *output;
	size_t output_len;
	int result;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <ansi-file> [columns]\n", argv[0]);
		fprintf(stderr, "  ansi-file: Path to ANSI art file\n");
		fprintf(stderr, "  columns:   Optional column width (default: auto-detect or 80)\n");
		return 1;
	}

	memset(&ctx, 0, sizeof(ctx));
	memset(&opts, 0, sizeof(opts));

	if (ansilove_init(&ctx, &opts) != 0) {
		fprintf(stderr, "ansilove_init failed: %s\n", ansilove_error(&ctx));
		return 1;
	}

	if (ansilove_loadfile(&ctx, argv[1]) != 0) {
		fprintf(stderr, "ansilove_loadfile failed: %s\n", ansilove_error(&ctx));
		ansilove_clean(&ctx);
		return 1;
	}

	opts.mode = ANSILOVE_MODE_TERMINAL;
	
	if (argc >= 3) {
		opts.columns = atoi(argv[2]);
		if (opts.columns < 1 || opts.columns > 255) {
			fprintf(stderr, "Invalid column count\n");
			ansilove_clean(&ctx);
			return 1;
		}
	}

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

	if (fwrite(output, 1, output_len, stdout) != output_len) {
		fprintf(stderr, "Write to stdout failed\n");
		ansilove_clean(&ctx);
		return 1;
	}

	ansilove_clean(&ctx);
	return 0;
}
