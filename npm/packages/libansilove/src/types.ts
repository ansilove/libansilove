/**
 * Permitted bit depths for ANSI font rendering. libansilove accepts 8-bit
 * characters by default and optionally supports 9-bit VGA fonts.
 */
export type RenderBits = 8 | 9;

/**
 * Optional rendering presets that mirror libansilove's C-side mode constants.
 *
 * - `"ced"` clamps width to 78 columns and draws with the classic CED
 *   gray-on-black palette.
 * - `"transparent"` designates palette index 0 as fully transparent in the
 *   output PNG so external backgrounds show through.
 * - `"workbench"` replaces the standard ANSI palette with the 16-color Amiga
 *   Workbench palette.
 *
 * These options are mutually exclusive; omit the field (or pass
 * `undefined`) to keep the default ANSI rendering path.
 */
export type RenderMode = "ced" | "transparent" | "workbench";

/**
 * Accepted column width overrides. `"auto"` defers to libansilove's
 * auto-detection (default), otherwise provide a positive integer.
 */
export type RenderColumns = number | "auto";

export interface RenderOptions {
	/**
	 * Explicit character column count for ANSI parsing. When omitted libansilove
	 * auto-detects the width (default 80). Set to `"auto"` to express that
	 * behaviour explicitly, or provide a positive integer to force wrapping.
	 */
	readonly columns?: RenderColumns;
	/**
	 * Character cell bit depth accepted by libansilove. ANSI art is almost
	 * always 8-bit; pass 9 for VGA extended fonts that rely on the high bit.
	 */
	readonly bits?: RenderBits;
	/**
	 * Rendering preset that selects specialised palettes or post-processing.
	 * See {@link RenderMode} for the available values. Leave undefined to retain
	 * standard ANSI colours.
	 */
	readonly mode?: RenderMode;
	/**
	 * Treat blinking colours as bright backgrounds (`true`) or keep the original
	 * blink behaviour (`false`). Mirrors the `-i/--icecolors` CLI flag.
	 */
	readonly iceColors?: boolean;
}

export type RenderInput = string | ArrayBuffer | ArrayBufferView;

export interface RenderResult {
	readonly png: Uint8Array;
}

export interface LibansiloveBindings {
	readonly module: EmscriptenModule;
	readonly version: string;
	/**
	 * Convert ANSI or related art into a PNG image using the compiled
	 * libansilove WebAssembly runtime.
	 *
	 * @param input Raw ANSI data as a string, ArrayBuffer, or typed array view.
	 * @param options Rendering customisations mirroring libansilove CLI flags.
	 * @returns The rendered PNG bytes and metadata.
	 */
	renderAnsi(input: RenderInput, options?: RenderOptions): RenderResult;
}

export interface LoadOptions {
	readonly locateFile?: (path: string) => string;
	readonly module?: ModuleOverrides;
}

export type ModuleOverrides = Record<string, unknown>;

export interface EmscriptenModule {
	cwrap(
		name: string,
		returnType: string | null,
		argTypes: string[],
	): (...args: unknown[]) => any;
	HEAPU8: Uint8Array;
	_malloc(size: number): number;
	_free(ptr: number): void;
}

export type ModuleFactory<T extends EmscriptenModule = EmscriptenModule> = (
	overrides?: ModuleOverrides & {
		locateFile?: (path: string) => string;
	},
) => Promise<T>;
