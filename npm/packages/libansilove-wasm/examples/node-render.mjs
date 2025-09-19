import { writeFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { load } from 'libansilove';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

async function main() {
  const wasm = await load();
  const { png } = wasm.renderAnsi('\u001b[1;35mHello from libansilove!\u001b[0m\r\n');

  const outputPath = path.resolve(__dirname, 'hello.png');
  await writeFile(outputPath, png);
  console.log(`Rendered ANSI art using libansilove ${wasm.version} -> ${outputPath}`);
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});
