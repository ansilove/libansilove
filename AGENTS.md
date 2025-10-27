# Repository Guidelines

## Project Structure & Module Organization
libansilove is a C library that converts ANSI and related art files to PNG. Core headers live in `include/`, while the implementation sits in `src/` with `loaders/` containing format-specific decoders and `fonts/` bundling built-in typefaces. Cross-platform fallbacks are under `compat/`. The `example/` directory shows how to invoke the API end-to-end, and `man/` provides installed manual pages. Dedicated fuzzing harnesses reside in `fuzz/`; build them only when running sanitizer-heavy tests.

### UTF-8 ANSI Terminal Mode
The `utf8ansi-terminal` branch adds terminal output mode that emits UTF-8 ANSI escape sequences instead of PNG images. Key files:
- **src/terminal.c** (lines 517-552): Gap-handling logic with background color preservation fix
- **include/ansilove.h**: `ansilove_terminal()` and `ansilove_terminal_emit()` APIs
- **test_terminal_output.c**: Test binary that outputs UTF-8 ANSI to stdout
- **ansilove-utf8ansi-ansee.c**: PNG converter wrapper (pipes to external `ansee` tool)
- **SESSION_NOTES_BACKGROUND_COLOR_FIX.md**: Complete technical details of the background color gap fix
- **BUILD_INSTRUCTIONS.md**: Build and usage guide for terminal mode
- **NEXT_SESSION_START_HERE.md**: Quick session handoff with current status

## Build, Test, and Development Commands
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`: configure the project after installing GD headers and libs.
- `cmake --build build`: compile shared and static variants of the library.
- `cmake --build build --target install`: install artifacts into the default prefix.
- `cmake -S fuzz -B fuzz-build`: set up clang-based libFuzzer targets.
- `cmake --build fuzz-build`: produce fuzz binaries such as `ansi` and `tundra`.

### Terminal Mode Build Commands
For the `utf8ansi-terminal` branch:
```bash
# Build library
rm -rf build && mkdir build && cd build
cmake .. && cmake --build .

# Build UTF-8 ANSI test binary
gcc -o ansilove-utf8ansi ../test_terminal_output.c \
    -I../include -I../src -L. -lansilove-static -lgd -lm

# Build PNG converter wrapper (requires ansee tool)
cd .. && gcc -o ansilove-utf8ansi-ansee ansilove-utf8ansi-ansee.c \
    -I./include -I./src -Lbuild -lansilove-static -lgd -lm
```

Test files are in `ansi_test_files/` (simple_colors.ans, box_drawing.ans, cursor_test.ans, palette.ans).

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

### Terminal Mode Testing
For the `utf8ansi-terminal` branch:
```bash
# Quick test
cd build
./ansilove-utf8ansi ../ansi_test_files/simple_colors.ans

# Test background color gap fix (critical regression test)
printf "\033[46mAB  CD\033[0m\n" > /tmp/test.ans
./ansilove-utf8ansi /tmp/test.ans
# Expected: [0m[38;2;170;170;170m[48;2;0;170;170mAB  CD[0m
# The [48;2;0;170;170m is cyan background - MUST cover the spaces between characters

# PNG conversion test (requires ansee tool)
cd .. && ./ansilove-utf8ansi-ansee /tmp/test.ans /tmp/test.png
```

**Critical Test**: The background color gap test verifies that spaces within colored regions preserve their background color instead of appearing black. This was the main bug fixed in commits ef875e9 and earlier.

## Commit & Pull Request Guidelines
- Commit messages follow sentence case with concise statements ending in a period (for example, `Update ChangeLog.`).
- Keep functional changes and formatting adjustments in separate commits and ensure files build before pushing.
- Pull requests should summarize the change, call out impacted loaders, and link tracking issues. Note which build or fuzz commands were run, and attach PNG outputs or screenshots when visual diffs help reviewers.

## Branch-Specific Context

### utf8ansi-terminal Branch
**Current Status** (as of Oct 24, 2025):
- ✅ Background color gap fix COMPLETED and VERIFIED (commits ef875e9, 14fc52d)
- ✅ All builds clean, tests passing
- 📝 2 commits ahead of origin (unpushed documentation)

**Recent Work**:
- Fixed critical bug where colored background regions would show black gaps between characters
- Root cause: Gap-handling used cursor positioning (`\033[nC`) which skipped emitting background colors
- Solution: Detect gaps with backgrounds and emit actual space characters with color codes preserved
- Critical pointer fix on src/terminal.c:537 to avoid dangling pointer to stack variable

**Key Implementation**: src/terminal.c lines 517-552
```c
// When gap detected between non-space characters:
// 1. Check if any cells in gap have background != 0
// 2. If yes: emit spaces with their background colors
// 3. If no: use cursor positioning for efficiency
// 4. CRITICAL: prev_cell = &grid->cells[r][g]; (NOT local variable!)
```

**Documentation Files**:
- SESSION_NOTES_BACKGROUND_COLOR_FIX.md: Complete technical analysis and fix details
- BUILD_INSTRUCTIONS.md: How to build and test terminal mode
- NEXT_SESSION_START_HERE.md: Quick session handoff and current status
- RENDERING_FIX_SUMMARY.md: Previous rendering fixes for CP437 characters

**Known Issues**: None - all tests passing

**Next Steps** (optional):
1. Push 2 unpushed commits to remote
2. Add automated regression tests for background color gaps
3. Performance optimization of gap-checking loop
4. Edge case testing with blink/invert attributes in colored gaps
