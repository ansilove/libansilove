
# Add reproducible flake env and WebAssembly target.
## Summary
- keep the dev shell ergonomic by wiring in Emscripten alongside cmake/clang and documenting the workflow for future agents
- teach the build to emit `libansilove.{js,wasm}` through a minimal GD-compatible raster backend so the existing loaders work in a browser
- expose a wasm facade (`ansilove_wasm_render_ansi` etc.), ship a Node smoke test, and serve a demo HTML page that renders ANSI art in Safari
- scaffold an `@effect-native/libansilove-wasm` package with TypeScript/ESM bindings (Node + browser), reproducible build scripts, and automated version syncing/bumping ready for publication, plus Bun demo integration
- ignore transient Nix/wasm artefacts and refresh the README/AGENTS notes with the exact commands for building, testing, and verifying in Safari

## Testing
- nix flake check
- nix develop --command bash scripts/test-wasm.sh
- node scripts/test-wasm-node.js build-wasm/wasm
- scripts/test-wasm-browser.sh (Safari with "Allow JavaScript from Apple Events")
- cd npm/libansilove-wasm && npm run build && npm test && npm run verify
