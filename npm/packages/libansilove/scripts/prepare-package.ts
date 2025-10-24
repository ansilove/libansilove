#!/usr/bin/env bun
import { access, copyFile, mkdir, rm } from "node:fs/promises";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const scriptDir = dirname(fileURLToPath(import.meta.url));
const pkgDir = resolve(scriptDir, "..");
const rootDir = resolve(pkgDir, "../../..");
const distDir = resolve(pkgDir, "dist");
const generatedDir = resolve(pkgDir, "generated");
const distGeneratedDir = resolve(distDir, "generated");

const REQUIRED_ARTIFACTS = [
	"libansilove.js",
	"libansilove.wasm",
];

async function ensureArtifacts() {
	await Promise.all(
		REQUIRED_ARTIFACTS.map(async (artifact) => {
			const file = join(generatedDir, artifact);
			try {
				await access(file);
			} catch {
				throw new Error(`expected wasm artifact at ${file}; run build-wasm first`);
			}
		}),
	);
}

async function copyAssets() {
	await rm(distGeneratedDir, { recursive: true, force: true });
	await mkdir(distGeneratedDir, { recursive: true });
	await Promise.all(
		REQUIRED_ARTIFACTS.map((artifact) =>
			copyFile(join(generatedDir, artifact), join(distGeneratedDir, artifact))
		),
	);

	await rm(join(distDir, "LICENSE"), { force: true });
	await rm(join(distDir, "README.md"), { force: true });
	await copyFile(resolve(rootDir, "LICENSE"), join(distDir, "LICENSE"));
	await copyFile(join(pkgDir, "README.md"), join(distDir, "README.md"));
}

async function main() {
	await ensureArtifacts();
	await mkdir(distDir, { recursive: true });
	await copyAssets();
	console.error(`Packaged artefacts staged in ${distDir}`);
}

await main();
