# Task: Clean Build System

**Status**: UNBLOCKED

## Problem
Repo has many test binaries, temp files cluttering the workspace.

## Solution
1. Add `.gitignore` for test binaries and build artifacts
2. Create `make clean` or cleanup script
3. Document build process in `.specs/utf8ansi/build.md`

## Build Requirements
```bash
# Minimal dependencies
gcc src/terminal.c src/loadfile.c src/init.c src/error.c \
    compat/strtonum.c compat/reallocarray.c \
    -o ansi_viewer viewer.c \
    -Iinclude -Isrc -Icompat -lm -D_GNU_SOURCE
```

## Definition of Done
- [ ] .gitignore covers all build artifacts
- [ ] Build script creates ansi_viewer binary
- [ ] Clean script removes temp files
- [ ] Build instructions in build.md
