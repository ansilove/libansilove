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

```ts
import { load } from "libansilove";

const lib = await load();
const { png } = lib.renderAnsi("Hello from libansilove!\r\n");
console.log(`PNG size: ${png.byteLength} bytes`);
```

The loader automatically works in browsers, workers, Node.js (>=18), Deno, and
Bun. If your bundler relocates the `.wasm` payload, provide a `locateFile`
override:

```ts
const lib = await load({
  locateFile: (path) => new URL(`/static/wasm/${path}`, window.location.href).toString(),
});
```

## API

- `load(options?)` – returns an object with the Emscripten module instance, the
  detected libansilove version, and a `renderAnsi()` helper that yields a
  `Uint8Array` containing the PNG bytes.
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
