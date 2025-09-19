import { createBindings } from "./bindings";
import type {
	LibansiloveBindings,
	LoadOptions,
	ModuleFactory,
	ModuleOverrides,
} from "./types";

const FACTORY_URL = new URL("../generated/libansilove.js", import.meta.url);

let cachedFactory: ModuleFactory | null = null;

async function resolveFactory(): Promise<ModuleFactory> {
	if (!cachedFactory) {
		const module = await import(FACTORY_URL.href);
		cachedFactory = (module as { default: ModuleFactory }).default;
	}
	return cachedFactory;
}

const buildLocateFile = (base: URL) => {
	const baseDir = new URL("./", base);
	return (path: string) => new URL(path, baseDir).href;
};

export async function load(
	options: LoadOptions = {},
): Promise<LibansiloveBindings> {
	const factory = await resolveFactory();
	const locateFile = options.locateFile ?? buildLocateFile(FACTORY_URL);
	const overrides: ModuleOverrides = { ...(options.module ?? {}), locateFile };
	const module = await factory(overrides);
	return createBindings(module);
}

export type {
	EmscriptenModule,
	LibansiloveBindings,
	LoadOptions,
	ModuleFactory,
	ModuleOverrides,
	RenderBits,
	RenderInput,
	RenderMode,
	RenderOptions,
	RenderScaleFactor,
	RenderResult,
} from "./types";
export { createBindings };
export {
	AnsiloveErrorCodes,
	AnsiloveFileReadError,
	AnsiloveFileWriteError,
	AnsiloveFormatError,
	AnsiloveGdError,
	AnsiloveInvalidParamError,
	AnsiloveMemoryError,
	AnsiloveRangeError,
	AnsiloveWasmError,
} from "./errors";

export default load;
