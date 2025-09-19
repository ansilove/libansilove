import { createBindings } from "./bindings";
import type { LibansiloveBindings, LoadOptions, ModuleFactory } from "./types";

const DEFAULT_LOCATE = (path: string) => new URL(`./wasm/${path}`, import.meta.url).href;

export async function loadBrowser(options: LoadOptions = {}): Promise<LibansiloveBindings> {
	const factoryModule: { default: ModuleFactory } = await import("./wasm/libansilove.browser.mjs");
	const factory = factoryModule.default;
	const locateFile = options.locateFile ?? DEFAULT_LOCATE;
	const overrides = { ...(options.module ?? {}), locateFile };
	const instance = await factory(overrides);
	return createBindings(instance);
}
