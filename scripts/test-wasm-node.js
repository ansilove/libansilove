#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const { TextEncoder } = require('util');
const { pathToFileURL } = require('url');

async function loadFactory(factoryPath) {
  const module = await import(pathToFileURL(factoryPath).href);
  const factory = module.default ?? module;
  if (typeof factory !== 'function') {
    throw new Error('libansilove.js did not export an initialization function');
  }
  return factory;
}

async function main() {
  const buildDir = path.resolve(process.argv[2] || 'build-wasm/wasm');
  const outputPath = path.resolve(process.argv[3] || path.join(buildDir, 'test-output.png'));
  const factoryPath = path.join(buildDir, 'libansilove.js');

  if (!fs.existsSync(factoryPath)) {
    throw new Error(`expected ${factoryPath} (run scripts/test-wasm.sh first)`);
  }

  const factory = await loadFactory(factoryPath);
  const Module = await factory();
  const getVersion = Module.cwrap('ansilove_wasm_version', 'string', []);
  const renderAnsi = Module.cwrap('ansilove_wasm_render_ansi', 'number', ['number', 'number', 'number', 'number', 'number', 'number']);
  const getPtr = Module.cwrap('ansilove_wasm_get_png_ptr', 'number', []);
  const getLen = Module.cwrap('ansilove_wasm_get_png_length', 'number', []);
  const freePng = Module.cwrap('ansilove_wasm_free_png', null, []);

  const sample = process.argv[4] || 'Hello from libansilove!\r\n';
  const encoder = new TextEncoder();
  const input = encoder.encode(sample);
  const ptr = Module._malloc(input.length);
  Module.HEAPU8.set(input, ptr);

  const rc = renderAnsi(ptr, input.length, 0, 0, 0, 0);
  Module._free(ptr);
  if (rc !== 0) {
    throw new Error(`ansilove_wasm_render_ansi returned ${rc}`);
  }

  const pngPtr = getPtr();
  const pngLength = getLen();
  if (!pngPtr || pngLength <= 8) {
    throw new Error(`unexpected PNG length ${pngLength}`);
  }

  const pngData = Module.HEAPU8.slice(pngPtr, pngPtr + pngLength);
  freePng();

  fs.mkdirSync(path.dirname(outputPath), { recursive: true });
  fs.writeFileSync(outputPath, Buffer.from(pngData));

  const version = getVersion();
  console.log(`Generated ${path.relative(process.cwd(), outputPath)} (${pngData.length} bytes) using libansilove wasm ${version}.`);
}

main().catch((error) => {
  console.error(error && error.stack ? error.stack : error);
  process.exit(1);
});
