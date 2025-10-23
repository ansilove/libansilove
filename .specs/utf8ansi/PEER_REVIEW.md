# UTF-8+ANSI Terminal Mode - Peer Review Guide

## Quick Start for Reviewers

### Prerequisites
- Linux terminal with 256-color support
- Git
- GCC compiler
- CMake (for building library)
- Optional: ansee (for PNG rendering)

### Setup

```bash
# Clone the repository
git clone https://github.com/effect-native/libansilove.git
cd libansilove
git checkout utf8ansi-terminal

# Build the library
mkdir -p build && cd build
cmake ..
make -j4
cd ..

# Build the CLI tools
gcc -o ansilove-utf8ansi viewer.c -Iinclude -Lbuild -lansilove -Wl,-rpath,$(pwd)/build
chmod +x ansilove-utf8ansi demo-utf8ansi.sh
```

### Test Files

Download the fire-43 ANSI art pack for testing:
```bash
# Available at: https://files.scene.org/view/resources/artpacks/2025/fire-43.zip
# Or use the included test files in ansi_test_files/
```

## Testing the Implementation

### 1. Basic Terminal Display

```bash
# Display DOS ANSI art in your Linux terminal
./ansilove-utf8ansi /path/to/file.ans
```

**Expected:**
- ANSI art renders with colors in terminal
- CP437 box-drawing characters display as Unicode equivalents
- No mojibake (garbled text)

### 2. Save to File

```bash
# Convert and save
./ansilove-utf8ansi /path/to/file.ans > output.utf8ansi

# View saved file
cat output.utf8ansi
```

**Expected:**
- File contains UTF-8 encoded text with ANSI SGR codes
- Can be viewed later with `cat`, `less -R`, etc.
- File size: typically 2-5× original .ans file size (due to UTF-8 multi-byte encoding)

### 3. Demo Script

```bash
# Run the interactive demo
./demo-utf8ansi.sh /path/to/file.ans
```

**Expected:**
- Shows all three use cases (print, save, ansee)
- Displays format details and usage examples
- Works with or without ansee installed

### 4. Fire-43 Collection Test

```bash
# Test with multiple files
for f in /path/to/fire-43/*.ANS; do
    echo -n "$(basename "$f"): "
    ./ansilove-utf8ansi "$f" >/dev/null 2>&1 && echo "✓" || echo "✗"
done
```

**Expected:**
- 26/26 files convert successfully ✓
- All files produce readable UTF-8+ANSI output

## What to Review

### Code Quality

1. **Terminal Backend** (`src/terminal.c`)
   - State machine for ANSI parsing
   - CP437 → Unicode conversion
   - DOS color → ANSI256 mapping
   - SGR sequence generation

2. **Color Mapping** (`src/dos_colors.h`)
   - `dos_color_to_ansi256()` function
   - `rgb_to_ansi256()` conversion
   - 16-color DOS palette

3. **CLI Tools**
   - `ansilove-utf8ansi` (viewer.c)
   - `demo-utf8ansi.sh`

### Output Verification

Compare outputs:
```bash
# Our output
./ansilove-utf8ansi file.ans > ours.utf8ansi

# Reference (if available)
cat-ans file.ans > reference.txt

# Check character content matches
diff <(grep -o '[^ ]' ours.utf8ansi) <(grep -o '[^ ]' reference.txt)
```

### Color Accuracy

Check that DOS colors map correctly:
```bash
# Extract ANSI codes from output
./ansilove-utf8ansi file.ans 2>/dev/null | grep -ao '38;5;[0-9]*m' | sort -u
```

**Expected ANSI256 codes for DOS palette:**
- DOS 0 (black) → ANSI 16
- DOS 1 (blue) → ANSI 19
- DOS 2 (green) → ANSI 34
- DOS 3 (cyan) → ANSI 37
- DOS 4 (red) → ANSI 124
- DOS 5 (magenta) → ANSI 127
- DOS 6 (brown) → ANSI 130
- DOS 7 (light gray) → ANSI 188
- DOS 8 (dark gray) → ANSI 59
- DOS 9 (light blue) → ANSI 63
- DOS 10 (light green) → ANSI 83
- DOS 11 (light cyan) → ANSI 87
- DOS 12 (light red) → ANSI 203
- DOS 13 (light magenta) → ANSI 207
- DOS 14 (yellow) → ANSI 227
- DOS 15 (white) → ANSI 231

## Known Issues

### 1. ansee PNG Rendering
- ansee uses anti-aliased TrueType rendering
- Creates gradient colors instead of pure DOS palette
- Not pixel-perfect compared to ansilove CLI PNG output
- Documented in `.specs/utf8ansi/ansee-comparison.md`

### 2. Bold SGR Warnings
- ansee may emit "Skipped graphics mode: [1]" warnings
- Bold attribute is parsed but may not render
- PNG is still created

## Success Criteria

✅ **Must Pass:**
1. Converts DOS ANSI to UTF-8+ANSI without crashes
2. CP437 characters render correctly in terminal
3. Colors display accurately (DOS palette → ANSI256)
4. Output can be saved and replayed with `cat`
5. Works with fire-43 collection (26/26 files) ✓

✅ **Nice to Have:**
1. ansee PNG rendering (with known limitations)
2. Performance <100ms for typical files
3. Memory efficient (no leaks)

## Documentation

Key files to review:
- `.specs/utf8ansi/README.md` - Quick start guide
- `.specs/utf8ansi/DONE.md` - Completion summary
- `.specs/utf8ansi/ansee-comparison.md` - PNG rendering analysis
- `demo-utf8ansi.sh` - Interactive demo

## Questions?

- GitHub: https://github.com/effect-native/libansilove
- Branch: `utf8ansi-terminal`
- Commits: See git log for detailed change history

## Example Session

```bash
$ cd libansilove
$ ./demo-utf8ansi.sh ~/Downloads/fire-43/ZIR-B1SQ.ANS

============================================
libansilove UTF-8+ANSI Terminal Mode Demo
============================================

Example 1: Print DOS ANSI to Linux terminal
-------------------------------------------
[colored ANSI art displays here]

Example 2: Save as .utf8ansi file
----------------------------------
Created: /tmp/demo.utf8ansi (4.5K, 49 lines)

Example 3: Render .utf8ansi to PNG with ansee
----------------------------------------------
Created: /tmp/demo.png (89K)

============================================
Format Details
============================================
Input:  DOS ANSI (.ans) - CP437 + DOS color codes
Output: Linux ANSI (.utf8ansi) - UTF-8 + ANSI 256-color SGR
...
```

Happy reviewing! 🎨
