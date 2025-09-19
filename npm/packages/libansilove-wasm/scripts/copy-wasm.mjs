import { copyFile, mkdir } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import { dirname, resolve } from 'node:path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const packageDir = resolve(__dirname, '..');
const workspaceRoot = resolve(packageDir, '..');
const repoRoot = resolve(workspaceRoot, '..');
const projectRoot = resolve(repoRoot, '..');
const sourceDir = resolve(projectRoot, 'build-wasm', 'wasm');
const distDir = resolve(packageDir, 'dist');

await mkdir(distDir, { recursive: true });

async function copyArtefact(file, target) {
  const source = resolve(sourceDir, file);
  try {
    await copyFile(source, target);
  } catch (error) {
    throw new Error(`Failed to copy ${file} from ${source}. Run 'scripts/test-wasm.sh' from the repository root first.\n${error}`);
  }
}

await copyArtefact('libansilove.browser.wasm', resolve(distDir, 'libansilove.browser.wasm'));
await copyArtefact('libansilove.browser.js', resolve(distDir, 'libansilove.browser.js'));
await copyArtefact('libansilove.node.wasm', resolve(distDir, 'libansilove.node.wasm'));
await copyArtefact('libansilove.node.cjs', resolve(distDir, 'libansilove.node.cjs'));

try {
  await copyFile(resolve(projectRoot, 'LICENSE'), resolve(distDir, 'LICENSE'));
} catch (error) {
  throw new Error(`Failed to copy repository LICENSE into npm dist.\n${error}`);
}
