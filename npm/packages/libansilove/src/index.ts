import { createBindings } from "./bindings";
import type {
	LibansiloveBindings,
	LoadOptions,
	ModuleFactory,
	ModuleOverrides,
	RenderInput,
	RenderOptions,
	RenderResult,
	EmscriptenModule,
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

export async function load(options: LoadOptions = {}): Promise<LibansiloveBindings> {
	const factory = await resolveFactory();
	const locateFile = options.locateFile ?? buildLocateFile(FACTORY_URL);
	const overrides: ModuleOverrides = { ...(options.module ?? {}), locateFile };
	const module = await factory(overrides);
	return createBindings(module);
}

export { createBindings };
export type {
	LibansiloveBindings,
	LoadOptions,
	RenderInput,
	RenderOptions,
	RenderResult,
	EmscriptenModule,
	ModuleFactory,
	ModuleOverrides,
} from "./types";

export default load;
