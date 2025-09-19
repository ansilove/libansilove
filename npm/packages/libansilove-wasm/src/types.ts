export interface RenderOptions {
  /** Override the number of columns (defaults to ANSI metadata). */
  columns?: number;
  /** Enable IceColor behavior. */
  icecolors?: boolean;
  /** Apply DOS aspect correction. */
  dos?: boolean;
  /** Scale factor multiplier (0 keeps original size). */
  scaleFactor?: number;
}

export interface LoadOptions {
  /**
   * Directory that contains `libansilove.wasm`. Defaults to the package `dist` folder.
   */
  wasmDir?: string;
  /**
   * Optional overrides passed through to the underlying Emscripten module factory.
   */
  moduleOverrides?: Record<string, unknown>;
}

export interface RenderResult {
  /** Raw PNG bytes rendered by libansilove. */
  png: Uint8Array;
}

export interface AnsiloveWasm {
  /** libansilove semantic version string. */
  version: string;
  /**
   * Render an ANSI input (string or bytes) into a PNG image.
   */
  renderAnsi(input: string | Uint8Array, options?: RenderOptions): RenderResult;
}
