export interface RenderOptions {
	/** Number of columns to assume when rendering (0 lets libansilove decide). */
	readonly columns?: number;
	/** Bits per pixel mode (0 = default). */
	readonly bits?: number;
	/** Rendering mode flag (0 = default). */
	readonly mode?: number;
	/** Whether to enable ICE colors (0 = disabled). */
	readonly iceColors?: number;
}

export type RenderInput = string | Uint8Array;

export interface RenderResult {
	/** PNG payload produced by the conversion. */
	readonly png: Uint8Array;
}

export interface LibansiloveBindings {
	readonly module: EmscriptenModule;
	readonly version: string;
	renderAnsi(input: RenderInput, options?: RenderOptions): RenderResult;
}

export interface LoadOptions {
	readonly locateFile?: (path: string) => string;
	readonly module?: ModuleOverrides;
}

export type ModuleOverrides = Record<string, unknown>;

export interface EmscriptenModule {
	cwrap(name: string, returnType: string | null, argTypes: string[]): (...args: unknown[]) => any;
	HEAPU8: Uint8Array;
	_malloc(size: number): number;
	_free(ptr: number): void;
}

export type ModuleFactory<T extends EmscriptenModule = EmscriptenModule> = (overrides?: ModuleOverrides & {
	locateFile?: (path: string) => string;
}) => Promise<T>;
