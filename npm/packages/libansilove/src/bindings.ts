import type {
	EmscriptenModule,
	LibansiloveBindings,
	RenderInput,
	RenderMode,
	RenderOptions,
	RenderResult,
} from "./types";

const encoder = new TextEncoder();

const DEFAULT_RENDER_ARGS: {
	readonly columns: number;
	readonly bits?: RenderOptions["bits"];
	readonly mode?: RenderMode;
	readonly iceColors: boolean;
} = {
	columns: 0,
	bits: undefined,
	mode: undefined,
	iceColors: false,
};

const toUint8Array = (input: RenderInput): Uint8Array => {
	if (typeof input === "string") {
		return encoder.encode(input);
	}

	if (ArrayBuffer.isView(input)) {
		const view = input as ArrayBufferView;
		if (view instanceof Uint8Array) {
			return view;
		}
		return new Uint8Array(view.buffer, view.byteOffset, view.byteLength);
	}

	if (input instanceof ArrayBuffer) {
		return new Uint8Array(input);
	}

	throw new TypeError("Unsupported render input type");
};

const normaliseColumns = (value: RenderOptions["columns"]): number => {
	if (value === undefined || value === "auto") {
		return DEFAULT_RENDER_ARGS.columns;
	}
	return value;
};

const normaliseMode = (mode: RenderMode | undefined): number => {
	switch (mode) {
		case "ced":
			return 1;
		case "transparent":
			return 2;
		case "workbench":
			return 3;
		case undefined:
			return 0;
		default:
			return 0;
	}
};

const toDisposable = (dispose: () => void) => ({ [Symbol.dispose]: dispose });

const allocateBuffer = (Module: EmscriptenModule, data: Uint8Array) => {
	const length = data.byteLength;
	const ptr = Module._malloc(length || 1);
	if (length > 0) {
		Module.HEAPU8.set(data, ptr);
	}
	return {
		ptr,
		[Symbol.dispose]() {
			Module._free(ptr);
		},
	};
};

export function createBindings(Module: EmscriptenModule): LibansiloveBindings {
	const getVersion = Module.cwrap(
		"ansilove_wasm_version",
		"string",
		[],
	) as () => string;
	const renderAnsiFn = Module.cwrap("ansilove_wasm_render_ansi", "number", [
		"number",
		"number",
		"number",
		"number",
		"number",
		"number",
	]) as (...args: number[]) => number;
	const getPngPtr = Module.cwrap(
		"ansilove_wasm_get_png_ptr",
		"number",
		[],
	) as () => number;
	const getPngLength = Module.cwrap(
		"ansilove_wasm_get_png_length",
		"number",
		[],
	) as () => number;
	const freePng = Module.cwrap(
		"ansilove_wasm_free_png",
		null,
		[],
	) as () => void;

	const renderAnsi = (
		input: RenderInput,
		options: RenderOptions = {},
	): RenderResult => {
		const data = toUint8Array(input);
		using buffer = allocateBuffer(Module, data);

		const cols = normaliseColumns(options.columns);
		const bits = options.bits ?? DEFAULT_RENDER_ARGS.bits;
		const mode = normaliseMode(options.mode ?? DEFAULT_RENDER_ARGS.mode);
		const ice = options.iceColors ?? DEFAULT_RENDER_ARGS.iceColors;

		const code = renderAnsiFn(
			buffer.ptr,
			data.byteLength,
			cols,
			bits ?? 0,
			mode,
			ice ? 1 : 0,
		);
		if (code !== 0) {
			throw new Error(
				`ansilove_wasm_render_ansi failed with exit code ${code}`,
			);
		}

		const pngPtr = getPngPtr();
		const pngLength = getPngLength();
		if (!pngPtr || pngLength <= 0) {
			throw new Error("libansilove returned an empty PNG buffer");
		}

		using pngHandle = toDisposable(freePng);
		const png = Module.HEAPU8.slice(pngPtr, pngPtr + pngLength);

		return { png };
	};

	return {
		module: Module,
		version: getVersion(),
		renderAnsi,
	};
}
