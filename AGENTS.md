# Repository Guidelines

## Project Structure & Module Organization
libansilove is a C library that converts ANSI and related art files to PNG. Core headers live in `include/`, while the implementation sits in `src/` with `loaders/` containing format-specific decoders and `fonts/` bundling built-in typefaces. Cross-platform fallbacks are under `compat/`. The `example/` directory shows how to invoke the API end-to-end, and `man/` provides installed manual pages. Dedicated fuzzing harnesses reside in `fuzz/`; build them only when running sanitizer-heavy tests.

## Build, Test, and Development Commands
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`: configure the project after installing GD headers and libs.
- `cmake --build build`: compile shared and static variants of the library.
- `cmake --build build --target install`: install artifacts into the default prefix.
- `cmake -S fuzz -B fuzz-build`: set up clang-based libFuzzer targets.
- `cmake --build fuzz-build`: produce fuzz binaries such as `ansi` and `tundra`.

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

## Commit & Pull Request Guidelines
- Commit messages follow sentence case with concise statements ending in a period (for example, `Update ChangeLog.`).
- Keep functional changes and formatting adjustments in separate commits and ensure files build before pushing.
- Pull requests should summarize the change, call out impacted loaders, and link tracking issues. Note which build or fuzz commands were run, and attach PNG outputs or screenshots when visual diffs help reviewers.
