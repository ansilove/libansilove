import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { createRequire } from 'node:module';
import { TextEncoder } from 'node:util';
import type { LoadOptions, RenderOptions, AnsiloveWasm, RenderResult } from './types.js';

const require = createRequire(import.meta.url);
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Minimal shape of the Emscripten factory output we rely on.
interface LibansiloveModule {
  readonly HEAPU8: Uint8Array;
  _malloc(size: number): number;
  _free(ptr: number): void;
  cwrap(
    name: string,
    returnType: 'number' | 'string' | null,
    argTypes: ('number')[],
  ): (...args: number[]) => number | string | void;
}

type LibansiloveFactory = (overrides?: Record<string, unknown>) => Promise<LibansiloveModule>;

function resolveWasmPath(file: string, wasmDir?: string): string {
  const base = wasmDir ?? __dirname;
  return path.join(base, file);
}

function toUint8Array(input: string | Uint8Array): Uint8Array {
  if (typeof input === 'string') {
    return new TextEncoder().encode(input);
  }
  return input;
}

async function loadFactory(): Promise<LibansiloveFactory> {
  const maybeFactory = require('./libansilove.cjs');
  if (typeof maybeFactory === 'function') {
    return maybeFactory as LibansiloveFactory;
  }
  if (maybeFactory && typeof maybeFactory.default === 'function') {
    return maybeFactory.default as LibansiloveFactory;
  }
  throw new Error('Failed to load libansilove wasm factory.');
}

export async function load(options: LoadOptions = {}): Promise<AnsiloveWasm> {
  const factory = await loadFactory();

  const module = await factory({
    locateFile: (file: string) => resolveWasmPath(file, options.wasmDir),
    ...(options.moduleOverrides ?? {}),
  });

  const renderAnsiFn = module.cwrap('ansilove_wasm_render_ansi', 'number', ['number', 'number', 'number', 'number', 'number', 'number']);
  const getPtr = module.cwrap('ansilove_wasm_get_png_ptr', 'number', []);
  const getLen = module.cwrap('ansilove_wasm_get_png_length', 'number', []);
  const freePng = module.cwrap('ansilove_wasm_free_png', null, []);
  const versionFn = module.cwrap('ansilove_wasm_version', 'string', []);

  const renderAnsi = (input: string | Uint8Array, renderOptions: RenderOptions = {}): RenderResult => {
    const data = toUint8Array(input);
    const ptr = module._malloc(data.length);
    try {
      module.HEAPU8.set(data, ptr);

      const result = renderAnsiFn(
        ptr,
        data.length,
        renderOptions.columns ?? 0,
        renderOptions.icecolors ? 1 : 0,
        renderOptions.dos ? 1 : 0,
        renderOptions.scaleFactor ?? 0,
      ) as number;

      if (result !== 0) {
        throw new Error(`ansilove_wasm_render_ansi failed with error code ${result}`);
      }

      const pngPtr = getPtr() as number;
      const pngLength = getLen() as number;
      if (!pngPtr || !pngLength) {
        throw new Error('ansilove_wasm_get_png_length returned zero bytes.');
      }

      const png = module.HEAPU8.slice(pngPtr, pngPtr + pngLength);
      freePng();
      return { png: new Uint8Array(png) };
    } finally {
      module._free(ptr);
    }
  };

  const version = versionFn() as string;

  return {
    version,
    renderAnsi,
  };
}

export type { AnsiloveWasm, LoadOptions, RenderOptions, RenderResult } from './types.js';
