#!/usr/bin/env bun
import { rm, mkdir, access, copyFile } from "node:fs/promises";
import { constants as fsConstants } from "node:fs";
import { resolve, dirname, join, basename } from "node:path";
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

const ensureFile = async (path: string) => {
  try {
    await access(path, fsConstants.F_OK);
  } catch {
    exitWith(`error: missing expected artifact ${path}`, 4);
  }
};

const main = async () => {
  const scriptDir = dirname(fileURLToPath(import.meta.url));
  const pkgDir = resolve(scriptDir, "..");
  const rootDir = resolve(pkgDir, "../../..");
  const buildDir = resolve(pkgDir, ".wasm-build");
  const artifactsDir = resolve(buildDir, "wasm");
  const outDir = resolve(pkgDir, "generated");

  requireCommand("emcmake", "enter via 'nix develop' or activate emsdk", 2);
  requireCommand("cmake", "ensure cmake is on PATH", 3);

  await rm(buildDir, { recursive: true, force: true });
  await mkdir(buildDir, { recursive: true });
  await mkdir(outDir, { recursive: true });

  const { $ } = Bun;

  await $`emcmake cmake -S ${rootDir} -B ${buildDir} -DANSILOVE_BUILD_WASM=ON`;
  await $`cmake --build ${buildDir} --target ansilove_wasm_browser ansilove_wasm_node`;

  const artifacts = [
    join(artifactsDir, "libansilove.browser.mjs"),
    join(artifactsDir, "libansilove.browser.wasm"),
    join(artifactsDir, "libansilove.node.cjs"),
    join(artifactsDir, "libansilove.node.wasm"),
  ];

  await Promise.all(artifacts.map((path) => ensureFile(path)));

  await Promise.all(
    artifacts.map((src) => copyFile(src, join(outDir, basename(src))))
  );

  console.error(`WASM artifacts copied to ${outDir}`);
};

await main();
