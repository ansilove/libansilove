import { access } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import { dirname, resolve } from 'node:path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const distDir = resolve(__dirname, '..', 'dist');

async function ensure(file) {
  try {
    await access(resolve(distDir, file));
  } catch (error) {
    throw new Error(`Missing dist artefact: ${file}\n${error}`);
  }
}

await Promise.all([
  ensure('index.js'),
  ensure('index.d.ts'),
  ensure('browser.js'),
  ensure('browser.d.ts'),
  ensure('libansilove.browser.js'),
  ensure('libansilove.browser.wasm'),
  ensure('libansilove.node.cjs'),
  ensure('libansilove.node.wasm'),
  ensure('LICENSE'),
]);
