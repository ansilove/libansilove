import type { AnsiloveWasm, RenderOptions, RenderResult } from './types.js';

type ModuleOverrides = Record<string, unknown>;

type Factory = (overrides?: ModuleOverrides) => Promise<LibansiloveModule>;

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

export interface BrowserLoadOptions {
  moduleOverrides?: ModuleOverrides;
  locateFile?: (file: string) => string;
}

function toUint8Array(input: string | Uint8Array): Uint8Array {
  if (typeof input === 'string') {
    return new TextEncoder().encode(input);
  }
  return input;
}

async function loadFactory(): Promise<Factory> {
  const previousProcess = (globalThis as { process?: unknown }).process;
  const hadProcess = Object.prototype.hasOwnProperty.call(globalThis, 'process');
  (globalThis as { process?: unknown }).process = undefined;

  try {
    const factoryModule = await import('./libansilove.js');
    const factory = (factoryModule as { default?: unknown }).default ?? factoryModule;
    if (typeof factory !== 'function') {
      throw new Error('Failed to load libansilove wasm factory for browser.');
    }
    return factory as Factory;
  } finally {
    if (hadProcess) {
      (globalThis as { process?: unknown }).process = previousProcess;
    } else {
      delete (globalThis as { process?: unknown }).process;
    }
  }
}

function defaultLocateFile(file: string): string {
  return new URL(`./${file}`, import.meta.url).href;
}

export async function loadBrowser(options: BrowserLoadOptions = {}): Promise<AnsiloveWasm> {
  const factory = await loadFactory();

  const module = await factory({
    locateFile: (file: string) => (options.locateFile ?? defaultLocateFile)(file),
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

export type { RenderOptions, RenderResult } from './types.js';
