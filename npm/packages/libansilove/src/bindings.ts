import type {
	EmscriptenModule,
	LibansiloveBindings,
	RenderInput,
	RenderOptions,
	RenderResult,
} from "./types";

const encoder = new TextEncoder();

const DEFAULT_RENDER_ARGS: Required<RenderOptions> = {
	columns: 0,
	bits: 0,
	mode: 0,
	iceColors: 0,
};

export function createBindings(Module: EmscriptenModule): LibansiloveBindings {
	const getVersion = Module.cwrap("ansilove_wasm_version", "string", []) as () => string;
	const renderAnsiFn = Module.cwrap("ansilove_wasm_render_ansi", "number", [
		"number",
		"number",
		"number",
		"number",
		"number",
		"number",
	]) as (...args: number[]) => number;
	const getPngPtr = Module.cwrap("ansilove_wasm_get_png_ptr", "number", []) as () => number;
	const getPngLength = Module.cwrap("ansilove_wasm_get_png_length", "number", []) as () => number;
	const freePng = Module.cwrap("ansilove_wasm_free_png", null, []) as () => void;

	const renderAnsi = (input: RenderInput, options: RenderOptions = {}): RenderResult => {
		const data = typeof input === "string" ? encoder.encode(input) : input;
		const ptr = Module._malloc(data.length || 1);
		if (data.length > 0) {
			Module.HEAPU8.set(data, ptr);
		}

		const cols = options.columns ?? DEFAULT_RENDER_ARGS.columns;
		const bits = options.bits ?? DEFAULT_RENDER_ARGS.bits;
		const mode = options.mode ?? DEFAULT_RENDER_ARGS.mode;
		const ice = options.iceColors ?? DEFAULT_RENDER_ARGS.iceColors;

		const code = renderAnsiFn(ptr, data.length, cols, bits, mode, ice);
		Module._free(ptr);
		if (code !== 0) {
			throw new Error(`ansilove_wasm_render_ansi failed with exit code ${code}`);
		}

		const pngPtr = getPngPtr();
		const pngLength = getPngLength();
		if (!pngPtr || pngLength <= 0) {
			throw new Error("libansilove returned an empty PNG buffer");
		}

		const png = Module.HEAPU8.slice(pngPtr, pngPtr + pngLength);
		freePng();

		return { png };
	};

	return {
		module: Module,
		version: getVersion(),
		renderAnsi,
	};
}
