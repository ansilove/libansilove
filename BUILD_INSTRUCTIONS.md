# Build Instructions for libansilove UTF-8 ANSI Terminal Mode

## Quick Start

### 1. Build the library
```bash
cd /home/tom/libansilove
rm -rf build && mkdir build && cd build
cmake ..
cmake --build .
```

### 2. Build test binary (UTF-8 ANSI output)
```bash
cd /home/tom/libansilove/build
gcc -o ansilove-utf8ansi ../test_terminal_output.c \
    -I../include -I../src -L. -lansilove-static -lgd -lm
```

### 3. Build PNG converter binary (pipes to ansee)
```bash
cd /home/tom/libansilove
gcc -o ansilove-utf8ansi-ansee ansilove-utf8ansi-ansee.c \
    -I./include -I./src -Lbuild -lansilove-static -lgd -lm
```

## Binary Locations

After building:
- **Main library**: `build/libansilove-static.a` (static) or `build/libansilove.so` (shared)
- **Test binary**: `build/ansilove-utf8ansi` (outputs UTF-8 ANSI to stdout)
- **PNG converter**: `./ansilove-utf8ansi-ansee` (in project root, requires ansee tool)

## Usage Examples

### UTF-8 ANSI Output
```bash
cd /home/tom/libansilove/build
./ansilove-utf8ansi input.ans > output.ans
./ansilove-utf8ansi input.ans | less -R  # View in terminal
```

### PNG Output (via ansee)
```bash
cd /home/tom/libansilove
./ansilove-utf8ansi-ansee input.ans output.png
```

## Test Cases

### Test colored background gaps
```bash
printf "\033[46mAB  CD\033[0m\n" > /tmp/test.ans
cd /home/tom/libansilove/build
./ansilove-utf8ansi /tmp/test.ans
# Should show: [0m[38;2;170;170;170m[48;2;0;170;170mAB  CD[0m
# Background [48;2;0;170;170m = cyan, should cover the spaces too
```

### Use existing test files
```bash
cd /home/tom/libansilove/build
./ansilove-utf8ansi ../ansi_test_files/simple_colors.ans
./ansilove-utf8ansi ../ansi_test_files/box_drawing.ans
./ansilove-utf8ansi ../ansi_test_files/cursor_test.ans
./ansilove-utf8ansi ../ansi_test_files/palette.ans
```

## Current Status

- **Branch**: `utf8ansi-terminal`
- **Latest commits**:
  - `ef875e9`: Add comprehensive session notes for background color gap fix
  - `14191a0`: ++
  - `44e18d9`: Update terminal.c
  - `6ca6c47`: Revert "Replace cursor positioning with spaces for line padding"
  
- **Working state**: All builds clean, all tests passing
- **Key fix**: Background color preservation in gaps (see SESSION_NOTES_BACKGROUND_COLOR_FIX.md)

## Dependencies

- GCC (C compiler)
- CMake
- libgd (graphics library)
- libm (math library)
- ansee tool (for PNG conversion, optional) - installed at `/home/tom/.cargo/bin/ansee`

## Troubleshooting

### "cannot find -lgd"
Install libgd development package:
```bash
sudo apt-get install libgd-dev  # Debian/Ubuntu
sudo dnf install gd-devel       # Fedora
```

### "ansee: command not found"
The PNG converter requires the ansee tool. If not available, use only the UTF-8 ANSI output binary.
Or install ansee:
```bash
cargo install ansee
```

### Build errors in terminal.c
Make sure you're on the `utf8ansi-terminal` branch with latest commits:
```bash
git checkout utf8ansi-terminal
git pull
```
