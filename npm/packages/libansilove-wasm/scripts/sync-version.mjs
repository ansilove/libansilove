import { readFile, writeFile } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import { dirname, resolve } from 'node:path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const packagePath = resolve(__dirname, '..', 'package.json');
const headerPath = resolve(__dirname, '..', '..', '..', 'include', 'ansilove.h');

const args = process.argv.slice(2);
const bumpPatch = args.includes('--bump-patch');

function parseLibraryVersion(contents) {
  const match = contents.match(/#define\s+ANSILOVE_VERSION\s+"([0-9]+\.[0-9]+\.[0-9]+)"/);
  if (!match) {
    throw new Error('Unable to parse ANSILOVE_VERSION from include/ansilove.h');
  }
  return match[1];
}

function compareSemver(a, b) {
  const pa = a.split('.').map(Number);
  const pb = b.split('.').map(Number);
  for (let i = 0; i < 3; i++) {
    if (pa[i] > pb[i]) return 1;
    if (pa[i] < pb[i]) return -1;
  }
  return 0;
}

function bumpPatchVersion(version) {
  const parts = version.split('.').map((value) => Number.parseInt(value, 10));
  if (parts.length !== 3 || parts.some(Number.isNaN)) {
    throw new Error(`Invalid semantic version: ${version}`);
  }
  parts[2] += 1;
  return parts.join('.');
}

const [packageJsonRaw, headerRaw] = await Promise.all([
  readFile(packagePath, 'utf8'),
  readFile(headerPath, 'utf8'),
]);

const pkg = JSON.parse(packageJsonRaw);
const libraryVersion = parseLibraryVersion(headerRaw);
let targetVersion = libraryVersion;

if (bumpPatch) {
  const baseline = pkg.version && compareSemver(pkg.version, libraryVersion) > 0 ? pkg.version : libraryVersion;
  targetVersion = bumpPatchVersion(baseline);
}

if (pkg.version === targetVersion) {
  console.log(`package.json already at ${targetVersion}`);
  process.exit(0);
}

pkg.version = targetVersion;
await writeFile(packagePath, `${JSON.stringify(pkg, null, 2)}\n`);
console.log(`Updated npm package version to ${targetVersion}`);
