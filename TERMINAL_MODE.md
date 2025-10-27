# UTF-8 ANSI Terminal Mode for libansilove

## Overview

The terminal mode extends libansilove to render ANSI art files directly to modern terminal emulators as UTF-8+ANSI SGR codes, rather than converting to PNG. This enables:

- **Instant display** of ANSI art without image generation
- **Terminal-native rendering** with proper colors and box-drawing characters
- **Small output size** compared to PNG
- **Scriptable output** for pipes and redirection

## Architecture

### Phase 1: Foundation (Completed)

#### 1.1 API Extension (ansilove.h)
- Added `ANSILOVE_MODE_TERMINAL = 4` enum value
- Added `ansilove_terminal()` - main entry point for parsing and conversion
- Added `ansilove_terminal_emit()` - returns UTF-8+ANSI output buffer

#### 1.2 CP437 Character Mapping (cp437_unicode.h)
- Complete 256-entry lookup table: CP437 byte → Unicode codepoint
- `cp437_to_utf8()` inline function: Encodes Unicode → UTF-8 bytes (1-4 bytes per char)
- **Fixed box-drawing mappings** for positions:
  - `0xB0-0xB7`: Shading and light vertical (░▒▓│┤╡╢╖)
  - `0xB8-0xBF`: Block fill and double vertical (█ ║╗╝╜╛┐)
  - `0xC0-0xCF`: All corner and T-junction variants

#### 1.3 DOS Color Palette (dos_colors.h)
- CGA/EGA 16-color palette with correct RGB values
- `dos_color_to_ansi256()` - Maps DOS colors 0-15 to ANSI256 codes
  - Colors 0-7: Direct mapping to standard colors
  - Colors 8-15: Bright variant mapping
- `rgb_to_ansi256()` - Alternative: Generic RGB → ANSI256 using 6×6×6 cube
- `dos_palette_init()` - Initialize color lookup table

### Phase 2: Backend Implementation (Completed)

#### 2.1 Terminal Backend Core (src/terminal.c)

**Grid Accumulation Structure:**
```c
struct terminal_cell {
    uint8_t character;      // CP437 byte
    uint32_t foreground;    // DOS color 0-15
    uint32_t background;    // DOS color 0-15
    bool bold;              // SGR 1
    bool blink;             // SGR 5
    bool invert;            // SGR 7
};

struct terminal_grid {
    struct terminal_cell **cells;  // 2D array
    int32_t max_column;
    int32_t max_row;
    int32_t width;
    int32_t height;
};
```

**Core Functions:**

1. **`ansilove_terminal(ctx, options)`**
   - Parses ANSI input buffer using state machine
   - Accumulates parsed cells in grid structure
   - Supports cursor positioning (CUP `H`/`f`)
   - Handles cursor navigation (`A`, `B`, `C`, `D`)
   - Processes Select Graphic Rendition (SGR `m`): bold, blink, invert, color
   - Returns 0 on success, populates `ctx->buffer` with grid data

2. **`terminal_emit_cell()`**
   - Converts single cell to UTF-8+ANSI codes
   - Emits SGR sequences only when attributes change
   - Handles state tracking to minimize output size
   - Returns UTF-8 bytes (1-3 for characters, + SGR codes)

3. **`ansilove_terminal_emit(ctx, length)`**
   - Iterates accumulated grid
   - Emits each cell with UTF-8+ANSI codes
   - Adds newlines at row boundaries
   - Returns output buffer pointer and total length

#### 2.2 ANSI Parser Integration
- Reuses cursor positioning logic from existing `ansi.c` loader
- State machine: `STATE_TEXT` → `STATE_SEQUENCE` → parse and execute
- Handles standard ANSI escape sequences:
  - **CUP** (Cursor Position): `ESC[row;colH` or `ESC[row;colf`
  - **CUA** (Cursor Up): `ESC[nA`
  - **CUD** (Cursor Down): `ESC[nB`
  - **CUF** (Cursor Forward): `ESC[nC`
  - **CUB** (Cursor Backward): `ESC[nD`
  - **SCP** (Save Cursor): `ESC7`
  - **RCP** (Restore Cursor): `ESC8`
  - **SGR** (Select Graphic Rendition): `ESC[n;n;nm`

## Usage

### Basic API Usage

```c
#include "ansilove.h"

struct ansilove_ctx ctx;
struct ansilove_options opts;
uint8_t *output;
size_t output_len;

ansilove_init(&ctx, &opts);
ansilove_loadfile(&ctx, "art.ans");

opts.mode = ANSILOVE_MODE_TERMINAL;
opts.columns = 80;  // Optional: default 80

ansilove_terminal(&ctx, &opts);
output = ansilove_terminal_emit(&ctx, &output_len);

fwrite(output, 1, output_len, stdout);

ansilove_clean(&ctx);
```

### Output Format

The output is a binary buffer containing:
1. UTF-8 encoded characters from CP437
2. ANSI SGR escape sequences for color/attributes
3. Newlines between rows

**Example sequence:**
```
ESC[0m       # Reset all
ESC[1m       # Bold (if needed)
ESC[38;5;16m # Foreground: DOS color 0 → ANSI256 16
ESC[48;5;22m # Background: DOS color 2 → ANSI256 22
E2 96 91     # UTF-8 for ░ (CP437 0xB0)
```

## Testing

### Unit Tests

```bash
gcc -I./include -I./src -std=c99 test_utf8_emit.c -o test_utf8
./test_utf8
```

**Validates:**
- CP437 0x41 → UTF-8 0x41 (ASCII 'A')
- CP437 0xB0 → UTF-8 0xE2 0x96 0x91 (░ Light shade)
- CP437 0xC0 → UTF-8 0xE2 0x94 0x94 (└ Corner)
- DOS colors 0-15 → correct ANSI256 codes

### Integration Test

```bash
# Requires full library build with GD support
gcc -I./include -I./src -I./compat test_terminal.c \
    src/terminal.c src/init.c src/error.c ... -lgd -lm -o test_terminal
./test_terminal input.ans output.ansi
```

## Known Limitations

1. **No image-based palette**: Terminal mode uses hardcoded DOS/CGA palette
2. **No PNG output**: Terminal mode is output-only (no image generation)
3. **Limited SGR support**: Supports bold, blink, invert; no extended colors
4. **Linear grid**: Assumes standard 80-column layout
5. **No SAUCE metadata**: Uses fixed column width (default 80)

## Future Enhancements (Phase 3+)

- [ ] Parse SAUCE metadata for intended width
- [ ] Extended color support (xterm-256color)
- [ ] Truecolor (24-bit RGB) output
- [ ] Terminal capability detection
- [ ] Line wrapping configuration
- [ ] Output streaming (on-the-fly rendering)

## File Structure

```
include/
  ansilove.h                 # API definitions
src/
  cp437_unicode.h            # CP437→Unicode table
  dos_colors.h               # Color palette & conversion
  terminal.c                 # Core implementation
test_utf8_emit.c             # Unit tests
test_terminal.c              # Integration test (skeleton)
```

## References

- CP437 specification: [Wikipedia](https://en.wikipedia.org/wiki/Code_page_437)
- ANSI escape codes: [ANSI/ECMA-48](https://en.wikipedia.org/wiki/ANSI_escape_code)
- UTF-8 encoding: [RFC 3629](https://tools.ietf.org/html/rfc3629)
- ANSI 256-color: [xterm-256color palette](https://en.wikipedia.org/wiki/Xterm_256color)
