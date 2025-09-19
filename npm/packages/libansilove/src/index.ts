import factory from "./libansilove.js";
import { createBindings } from "./bindings";
import type { LibansiloveBindings, LoadOptions, ModuleFactory } from "./types";

const DEFAULT_LOCATE_FILE = (path: string) => new URL(`./${path}`, import.meta.url).href;

export async function load(options: LoadOptions = {}): Promise<LibansiloveBindings> {
	const locateFile = options.locateFile ?? DEFAULT_LOCATE_FILE;
	const overrides = { ...(options.module ?? {}), locateFile };
	const module = await (factory as ModuleFactory)(overrides);
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
