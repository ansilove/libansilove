#!/usr/bin/env bun
import { constants as fsConstants } from "node:fs";
import { access, mkdir, readdir, rename, rm } from "node:fs/promises";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const exitWith = (message: string, code = 1) => {
	console.error(message);
	process.exit(code);
};

const requireCommand = (cmd: string, hint: string, code: number) => {
	if (!Bun.which(cmd)) {
		exitWith(`error: ${cmd} is required (${hint}).`, code);
	}
};

const main = async () => {
	const scriptDir = dirname(fileURLToPath(import.meta.url));
	const pkgDir = resolve(scriptDir, "..");
	const rootDir = resolve(pkgDir, "../../..");
	const outDir = resolve(pkgDir, "generated");

	requireCommand("emcc", "enter via 'nix develop' or activate emsdk", 2);

	await mkdir(outDir, { recursive: true });
	const existing = await readdir(outDir).catch(() => []);
	await Promise.all(
		existing
			.filter((entry) => entry !== "README.md")
			.map((entry) =>
				rm(join(outDir, entry), { recursive: true, force: true }).catch(
					() => undefined,
				),
			),
	);

	const includeFlags = ["include", "src", "compat", "wasm"].map(
		(dir) => `-I${resolve(rootDir, dir)}`,
	);

	const commonSources = [
		"src/clean.c",
		"src/drawchar.c",
		"src/fonts.c",
		"src/error.c",
		"src/loadfile.c",
		"src/init.c",
		"src/output.c",
		"src/savefile.c",
		"src/loaders/ansi.c",
		"src/loaders/artworx.c",
		"src/loaders/binary.c",
		"src/loaders/icedraw.c",
		"src/loaders/pcboard.c",
		"src/loaders/tundra.c",
		"src/loaders/xbin.c",
		"compat/strtonum.c",
		"compat/reallocarray.c",
		"wasm/ansilove_wasm.c",
		"wasm/minigd.c",
	].map((file) => resolve(rootDir, file));

	const exportedFunctions = [
		"_ansilove_wasm_version",
		"_ansilove_wasm_render_ansi",
		"_ansilove_wasm_get_png_ptr",
		"_ansilove_wasm_get_png_length",
		"_ansilove_wasm_free_png",
		"_malloc",
		"_free",
	];

	const exportedRuntimeMethods = ["cwrap", "ccall", "UTF8ToString"];

	const makeListFlag = (values: string[]) =>
		`[${values.map((value) => `'${value}'`).join(",")}]`;

	const buildTarget = async (options: {
		outputName: string;
		exportName: string;
		environment: string;
	}) => {
		const { outputName, exportName, environment } = options;
		const outputPath = join(outDir, outputName);
		const wasmPath = outputPath.replace(/\.js$/, ".wasm");
		const args = [
			...includeFlags,
			"-O3",
			"-DANSILOVE_WASM",
			"-sMODULARIZE=1",
			"-sEXPORT_ES6=1",
			"-sUSE_ES6_IMPORT_META=1",
			`-sEXPORT_NAME=${exportName}`,
			`-sEXPORTED_FUNCTIONS=${makeListFlag(exportedFunctions)}`,
			`-sEXPORTED_RUNTIME_METHODS=${makeListFlag(exportedRuntimeMethods)}`,
			"-sALLOW_MEMORY_GROWTH=1",
			`-sENVIRONMENT=${environment}`,
			...commonSources,
			"-o",
			outputPath,
		];

		const proc = Bun.spawn(["emcc", ...args], {
			stdout: "inherit",
			stderr: "inherit",
		});
		const code = await proc.exited;
		if (code !== 0) {
			exitWith(
				`error: emcc exited with code ${code}`,
				typeof code === "number" ? code : 1,
			);
		}

		try {
			await Promise.all([
				access(outputPath, fsConstants.F_OK),
				access(wasmPath, fsConstants.F_OK),
			]);
		} catch {
			exitWith(`error: expected artifacts ${outputPath} and ${wasmPath}`, 4);
		}
	};

	await buildTarget({
		outputName: "libansilove.js",
		exportName: "libansilove",
		environment: "web,webview,worker,node,shell",
	});

	console.error(`WASM artifacts written to ${outDir}`);
};

await main();
