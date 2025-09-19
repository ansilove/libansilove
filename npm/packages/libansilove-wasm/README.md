# @effect-native/libansilove-wasm

WebAssembly build of [libansilove](https://www.ansilove.org) packaged for Node.js
and modern bundlers. The package re-exports the generated Emscripten module along
with a small TypeScript helper that renders ANSI art to PNG buffers.

## Usage

```ts
import { load } from '@effect-native/libansilove-wasm';

const wasm = await load();
const result = wasm.renderAnsi('Hello from libansilove!\r\n');
console.log(`Generated ${result.png.length} bytes from libansilove ${wasm.version}`);
```

### Rendering options

```ts
const output = wasm.renderAnsi(ansiText, {
  columns: 80,
  icecolors: true,
  dos: false,
  scaleFactor: 2,
});
```

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
3. Verify the distribution before publishing:
   ```
   npm run verify
   ```

`npm run sync-version` keeps the package version aligned with `ANSILOVE_VERSION`
inside `include/ansilove.h`. Use `npm run version:patch` to bump the patch level
when preparing releases.
