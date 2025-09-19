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

await copyArtefact('libansilove.wasm', resolve(distDir, 'libansilove.wasm'));
await copyArtefact('libansilove.js', resolve(distDir, 'libansilove.js'));

try {
  await copyFile(resolve(projectRoot, 'LICENSE'), resolve(distDir, 'LICENSE'));
} catch (error) {
  throw new Error(`Failed to copy repository LICENSE into npm dist.\n${error}`);
}
