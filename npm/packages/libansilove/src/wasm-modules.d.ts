declare module "./wasm/libansilove.browser.mjs" {
	import type { ModuleFactory } from "./types";
	const factory: ModuleFactory;
	export default factory;
}

declare module "./wasm/libansilove.node.mjs" {
	import type { ModuleFactory } from "./types";
	const factory: ModuleFactory;
	export default factory;
}
