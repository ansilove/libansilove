# libansilove

WebAssembly build of [libansilove](https://www.ansilove.org) packaged for Node.js
and modern bundlers. The package re-exports the generated Emscripten module along
with a small TypeScript helper that renders ANSI art to PNG buffers.

## Usage (Node.js)

```ts
import { load } from 'libansilove';

async function main() {
  const wasm = await load();
  const { png } = wasm.renderAnsi('\u001b[1;36mHello from libansilove!\u001b[0m\r\n');
  console.log(`libansilove ${wasm.version} produced ${png.byteLength} bytes.`);
}

main();
```

See `examples/node-render.mjs` for a full script that writes the PNG to disk.

## Usage (browser / bundlers)

```ts
import { loadBrowser } from 'libansilove/browser';

const wasm = await loadBrowser();
const { png } = wasm.renderAnsi('Hello from libansilove!\r\n');
const blob = new Blob([png], { type: 'image/png' });
const url = URL.createObjectURL(blob);
```

By default `loadBrowser` resolves the wasm binary via `new URL('./libansilove.wasm',
import.meta.url)`, so bundlers that understand `import.meta.url` will copy the
asset automatically.

## Development

1. Build the wasm artefacts from the repository root:
   ```
   nix develop --command bash scripts/test-wasm.sh
   ```
2. Install dependencies and build the package:
   ```
   cd npm/libansilove-wasm
   npm install
   npm run build
   ```
3. Run the example test and verification suite before publishing:
   ```
   npm test
   npm run verify
   ```

`npm run sync-version` keeps the package version aligned with `ANSILOVE_VERSION`
inside `include/ansilove.h`. Use `npm run version:patch` to bump the patch level
when preparing releases.
