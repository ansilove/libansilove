#include "ansilove.h"
#include <emscripten/emscripten.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct wasm_png_result {
	uint8_t *data;
	size_t length;
};

static struct wasm_png_result wasm_result;

static void
wasm_clear_result(void)
{
	if (wasm_result.data != NULL) {
		free(wasm_result.data);
		wasm_result.data = NULL;
	}
	wasm_result.length = 0;
}

static void
wasm_stash_result(struct ansilove_ctx *ctx)
{
	wasm_result.data = ctx->png.buffer;
	wasm_result.length = (size_t)ctx->png.length;
	ctx->png.buffer = NULL;
	ctx->png.length = 0;
}

EMSCRIPTEN_KEEPALIVE
const char *
ansilove_wasm_version(void)
{
	return ANSILOVE_VERSION;
}

EMSCRIPTEN_KEEPALIVE
int
ansilove_wasm_render_ansi(uintptr_t data_ptr, size_t length, int columns,
	int bits, int mode, int icecolors, int dos, int scale_factor)
{
	struct ansilove_ctx ctx;
	struct ansilove_options options;
	int result;

	wasm_clear_result();

	if (ansilove_init(&ctx, &options) != 0)
		return -ANSILOVE_INVALID_PARAM;

	ctx.buffer = (uint8_t *)data_ptr;
	ctx.length = length;
	ctx.maplen = 0;

	if (columns > 0)
		options.columns = columns;
	if (bits == 8 || bits == 9)
		options.bits = (uint8_t)bits;
	if (mode >= ANSILOVE_MODE_CED && mode <= ANSILOVE_MODE_WORKBENCH)
		options.mode = (uint8_t)mode;
	options.icecolors = icecolors ? true : false;
	options.dos = dos ? true : false;
	if (scale_factor >= 0 && scale_factor <= 8)
		options.scale_factor = (uint8_t)scale_factor;

	result = ansilove_ansi(&ctx, &options);
	if (result != 0) {
		int error = ctx.error;
		ansilove_clean(&ctx);
		return -error;
	}

	wasm_stash_result(&ctx);
	ansilove_clean(&ctx);
	return 0;
}

EMSCRIPTEN_KEEPALIVE
uintptr_t
ansilove_wasm_get_png_ptr(void)
{
	return (uintptr_t)wasm_result.data;
}

EMSCRIPTEN_KEEPALIVE
size_t
ansilove_wasm_get_png_length(void)
{
	return wasm_result.length;
}

EMSCRIPTEN_KEEPALIVE
void
ansilove_wasm_free_png(void)
{
	wasm_clear_result();
}
