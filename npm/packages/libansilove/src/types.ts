export interface RenderOptions {
	readonly columns?: number;
	readonly bits?: number;
	readonly mode?: number;
	readonly iceColors?: number;
}

export type RenderInput = string | Uint8Array;

export interface RenderResult {
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

export type ModuleFactory<T extends EmscriptenModule = EmscriptenModule> = (
	overrides?: ModuleOverrides & {
		locateFile?: (path: string) => string;
	},
) => Promise<T>;
