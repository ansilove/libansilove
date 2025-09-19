# libansilove

> Universal WebAssembly bindings for [libansilove](https://www.ansilove.org/).

## Install

```bash
npm install libansilove
# or
pnpm add libansilove
# or
bun add libansilove
```

## Usage

### Node.js / Bun

```ts
import { loadNode } from "libansilove";

const lib = await loadNode();
const { png } = lib.renderAnsi("Hello from libansilove!\r\n");
console.log(`PNG size: ${png.byteLength} bytes`);
```

### Browser / Worker

```ts
import { loadBrowser } from "libansilove";

const lib = await loadBrowser();
const { png } = lib.renderAnsi("Hello from libansilove!\r\n");
const blob = new Blob([png], { type: "image/png" });
document.querySelector("img")!.src = URL.createObjectURL(blob);
```

Both loaders accept an optional `locateFile` override if your bundler moves the
`.wasm` payloads:

```ts
const lib = await loadBrowser({
  locateFile: (path) => new URL(`/static/wasm/${path}`, window.location.href).toString(),
});
```

## API

- `loadNode(options?)` – returns an object with the Emscripten module instance,
  the detected libansilove version, and a `renderAnsi()` helper that yields a
  `Uint8Array` containing the PNG bytes.
- `loadBrowser(options?)` – same contract as `loadNode` but tuned for browser,
  worker, and Deno style environments.
- `createBindings(module)` – wrap a raw Emscripten module if you are managing
  instantiation yourself.

See the generated TypeScript definitions in `dist/index.d.ts` for the full type
surface.

## Building

```bash
bun install
bun run build
bun run verify
```

The build performs three steps:

1. Compile libansilove to WebAssembly via Emscripten (`build:wasm`).
2. Emit typed JavaScript entry points (`build:ts`).
3. Copy runtime artefacts and metadata into `dist/` (`prepare:package`).

`bun run verify` loads the packaged output and renders a short ANSI snippet to
confirm everything is wired correctly. Use `npm pack` (after running `bun run
build`) to inspect the final tarball.
