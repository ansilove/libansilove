import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { load } from '../dist/index.js';
import { strict as assert } from 'node:assert';

async function main() {
  const __filename = fileURLToPath(import.meta.url);
  const __dirname = path.dirname(__filename);
  const wasmDir = path.resolve(__dirname, '..', 'dist');

  const wasm = await load({ wasmDir });
  assert.match(wasm.version, /^\d+\.\d+\.\d+$/, 'Version should be semver');

  const result = wasm.renderAnsi('Hello from libansilove!\r\n');
  assert.ok(result.png.byteLength > 16, 'PNG output should be non-empty');
  assert.equal(result.png[0], 0x89, 'PNG signature should start with 0x89');
  assert.equal(result.png[1], 0x50, 'PNG signature should include P');
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
