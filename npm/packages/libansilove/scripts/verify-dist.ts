#!/usr/bin/env bun
const moduleURL = new URL("../dist/index.js", import.meta.url).href;

async function main() {
	const mod = await import(moduleURL) as { load: typeof import("../src/index").load };
	const bindings = await mod.load();
	const { png } = bindings.renderAnsi("Hello from libansilove!\r\n");
	if (!png.byteLength) {
		throw new Error("renderAnsi returned an empty buffer");
	}
	console.error(`Verified libansilove wasm ${bindings.version} (${png.byteLength} bytes)`);
}

await main();
