# Repository Guidelines

## Project Structure & Module Organization
libansilove is a C library that converts ANSI and related art files to PNG. Core headers live in `include/`, while the implementation sits in `src/` with `loaders/` containing format-specific decoders and `fonts/` bundling built-in typefaces. Cross-platform fallbacks are under `compat/`. The `example/` directory shows how to invoke the API end-to-end, and `man/` provides installed manual pages. Dedicated fuzzing harnesses reside in `fuzz/`; build them only when running sanitizer-heavy tests.

## Build, Test, and Development Commands
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`: configure the project after installing GD headers and libs.
- `cmake --build build --parallel`: compile shared and static variants of the library.
- `cmake --build build --target install --parallel`: install artifacts into the default prefix.
- `cmake -S fuzz -B fuzz-build`: set up clang-based libFuzzer targets.
- `cmake --build fuzz-build --parallel`: produce fuzz binaries such as `ansi` and `tundra`.
- `nix develop`: enter the flake-backed dev shell with clang, pkg-config, GD, and platform-appropriate debugger preinstalled.
- `nix build`: build the default flake package (shared/static library) without touching your host toolchain.
- `nix flake check`: validate the flake packages and dev shell evaluate cleanly across supported systems.
- `nix develop --command bash scripts/test-wasm.sh`: run the Bun-driven `emcc` build and assert the JS/WASM artefacts exist.
- `scripts/test-wasm-browser.sh`: launch a local server and exercise the Safari smoke test end-to-end.
- `python3 -m http.server 8765 --directory example/wasm`: manual alternative to serve the browser demo (after running `scripts/test-wasm.sh`).
- `cd npm/packages/libansilove && bun install && bun run scripts/build-wasm.bun.ts`: rebuild the npm artefacts.
- `node scripts/test-wasm-node.js npm/packages/libansilove/generated`: rerun the Node smoke test on an existing build.
- `npm version patch` within `npm/packages/libansilove` (once publishing is wired) to bump metadata in step with `ANSILOVE_VERSION`.

## Coding Style & Naming Conventions
- Target C99 with the default warning set (`-Wall -Wextra -pedantic`).
- Indent with tabs for blocks; align wrapped parameters using spaces as needed, and avoid trailing whitespace.
- Public APIs stay in `include/ansilove.h` and use the `ansilove_*` prefix; internal helpers remain lowercase with underscores and `static` linkage.
- Mirror existing filenames (`loadfile.c`, `savefile.c`) when adding new modules or loaders.

## Testing Guidelines
- There is no unit-test harness; validate behavior with the example app and fuzzers.
- After building, run `build/example/ansilove_example <input.ans>` to confirm PNG output.
- For fuzzing, execute `./fuzz-build/ansi -runs=10000 corpus/` (seed the corpus with representative art files). Investigate sanitizer reports immediately and add reproducer samples.
- Ensure new formats or options ship with updated example inputs or fuzz seeds that exercise the paths.
- If you touch the flake, rerun `nix build` and `nix flake check` and commit the updated `flake.lock` (keep changes reproducible).
- For the wasm target, keep `scripts/test-wasm.sh` passing under `nix develop`; it rebuilds the Bun-driven artefacts via `emcc` every run.
- Safari automation relies on “Allow JavaScript from Apple Events” in Preferences ▸ Advanced. The helper scripts copy `libansilove.browser.{mjs,wasm}` into `example/wasm/` so you can smoke-test via the provided browser script or a manual `python3 -m http.server` session.

## Commit & Pull Request Guidelines
- Commit messages follow sentence case with concise statements ending in a period (for example, `Update ChangeLog.`).
- Keep functional changes and formatting adjustments in separate commits and ensure files build before pushing.
- Pull requests should summarize the change, call out impacted loaders, and link tracking issues. Note which build or fuzz commands were run, and attach PNG outputs or screenshots when visual diffs help reviewers.
