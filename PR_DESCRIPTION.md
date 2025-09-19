
# Add reproducible flake env and WebAssembly target.
## Summary
- keep the dev shell ergonomic by wiring in Emscripten alongside clang/cmake and documenting the workflow for future agents
- compile libansilove to a single universal `libansilove.{js,wasm}` bundle and surface a small TypeScript API around `renderAnsi`
- stand up the `libansilove` npm workspace with Bun-based build scripts, packaging helpers, smoke tests, and publishable dist output
- prune the legacy shell helpers/demo assets in favour of the new universal loader and move the Safari JXA harness into the Bun demo package
- refresh README/AGENTS with the simplified commands for building, testing, and verifying the wasm pipeline

## Testing
- nix flake check
- nix develop --command bash -lc 'cd npm/packages/libansilove && bun run build'
- cd npm/packages/libansilove && bun test
- cd npm/packages/libansilove && bun run verify
