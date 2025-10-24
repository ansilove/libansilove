# DOS CP437 to UTF-8 Character Mapping Analysis

## Summary

The `H4-2017.ANS` file contains **62 unique characters** from the DOS Code Page 437 character set. These are mapped to Unicode equivalents in the UTF-8 ANSI terminal output.

## Character Mappings Found in H4-2017.ANS

### ASCII Printable Characters (0x20-0x7E)
These map directly to ASCII/Unicode:
- Space (0x20), `!`, `"`, `#`, `$`, `%`, `&`, `'`, `(`, `)`, `*`, `+`, `,`, `-`, `.`, `/`
- Digits 0-9 (0x30-0x39)
- Punctuation: `:`, `;`, `<`, `=`, `>`, `?`
- Letters: `@`, `A`-`Z`, `[`, `\`, `]`, `^`, `_`
- Backtick: `` ` ``
- Letters: `a`-`z`, `{`, `|`, `}`, `~`

### Extended CP437 Graphics Characters (0x80-0xFF)

#### Box Drawing
- 0xB0: `░` (Light Shade) → U+2591
- 0xB1: `▒` (Medium Shade) → U+2592
- 0xB2: `▓` (Dark Shade) → U+2593
- 0xB3: `│` (Box Vertical) → U+2502
- 0xBF: `┐` (Box Top-Right) → U+2510
- 0xC0: `└` (Box Bottom-Left) → U+2514
- 0xC4: `─` (Box Horizontal) → U+2500
- 0xC8: `╚` (Box Bottom-Left Double) → U+255A
- 0xCD: `═` (Box Horizontal Double) → U+2550
- 0xD9: `┘` (Box Bottom-Right) → U+2518
- 0xDA: `┌` (Box Top-Left) → U+250C
- 0xDB: `█` (Full Block) → U+2588
- 0xDC: `▄` (Lower Half Block) → U+2584
- 0xDD: `▌` (Left Half Block) → U+258C
- 0xDE: `▐` (Right Half Block) → U+2590
- 0xDF: `▀` (Upper Half Block) → U+2580

#### Accented Letters
- 0x80: `Ç` → U+00C7
- 0x81: `ü` → U+00FC
- 0x82: `é` → U+00E9
- 0x83: `â` → U+00E2
- 0x84: `ä` → U+00E4
- 0x85: `à` → U+00E0
- 0x86: `å` → U+00E5
- 0x87: `ç` → U+00E7
- 0x88: `ê` → U+00EA
- 0x89: `ë` → U+00EB
- 0x8A: `è` → U+00E8
- 0x8B: `ï` → U+00EF
- 0x8C: `î` → U+00EE
- 0x8D: `ì` → U+00EC
- 0x8E: `Ä` → U+00C4
- 0x8F: `Å` → U+00C5
- 0x90: `É` → U+00C9
- 0x91: `æ` → U+00E6
- 0x92: `Æ` → U+00C6
- 0x93: `ô` → U+00F4
- 0x94: `ö` → U+00F6
- 0x95: `ò` → U+00F2
- 0x96: `û` → U+00FB
- 0x97: `ù` → U+00F9
- 0x98: `ÿ` → U+00FF ⭐ *This was the character rendering bug - was showing as ⌂*
- 0x99: `Ö` → U+00D6
- 0x9A: `Ü` → U+00DC
- 0x9B: `¢` → U+00A2
- 0x9C: `£` → U+00A3
- 0x9D: `¥` → U+00A5
- 0x9E: `₧` → U+20A7
- 0x9F: `ƒ` → U+0192

#### Symbols
- 0x7F: `⌂` (House) → U+2302
- 0xA5: `Ñ` → U+00D1 (was incorrectly labeled as ¥)
- 0xA6: `ª` → U+00AA
- 0xA9: `⌐` → U+2310
- 0xAA: `¬` → U+00AC
- 0xE2: `Γ` → U+0393 (was Σ in table)
- 0xF9: `∙` → U+2219
- 0xFA: `·` → U+00B7
- 0xFD: `²` → U+00B2
- 0xFE: `■` → U+25A0

#### Control Characters (rendered as visible CP437 graphics) ✓ FIXED
These were previously ignored but are valid DOS ANSI art characters:
- 0x06: `♠` (Spade) → U+2660 (26 occurrences in H4-2017.ANS)
- 0x0E: `♬` (Music Note) → U+266C (13 occurrences in H4-2017.ANS)
- 0x16: `▬` (Box Horizontal) → U+25AC (17 occurrences in H4-2017.ANS)
- 0x1C: `∟` (Right Angle) → U+221F (1 occurrence in H4-2017.ANS)

## Test File Generation

A comprehensive test file (`test_all_chars.ans`) has been created with all 256 DOS ASCII characters organized in a 16×16 grid:
- Rows 0-15 display character ranges 0x00-0x0F through 0xF0-0xFF
- Box drawing characters provide visual separators
- Control characters that would break layout are shown as placeholders (`.`)
- SAUCE metadata specifies 80×32 display dimensions

### Test Files Available
- `/home/tom/libansilove/test_all_chars.ans` - Master test file
- UTF-8 ANSI output: `/tmp/all_chars_utf8ansi.txt`
- PNG output: `/tmp/all_chars_png.png`

## Key Findings

1. **Control Characters Bug (FIXED)**: DOS art files use control characters (0x06, 0x0E, 0x16, 0x1C) as visible graphics
   - Root cause: Parser was filtering out all characters < 0x20 as non-displayable
   - Status: ✓ FIXED - changed filter from >= 0x20 to >= 0x01, added explicit 0x1A EOF check
   - Now correctly renders: ♠, ♬, ▬, ∟

2. **Character 0x98 Bug (FIXED)**: Was rendering as ⌂ (house, U+2302) but should be ÿ (U+00FF)
   - Root cause: Incorrect table indexing in cp437_to_utf8 function
   - Status: ✓ FIXED - now correctly shows as ÿ (43 occurrences in H4-2017.ANS)

3. **UTF-8 Encoding**: All characters are correctly converted to UTF-8 multibyte sequences:
   - ASCII (0x00-0x7F): 1 byte
   - Latin Extended (0x80-0xFF): 2-3 bytes
   - Symbols: 3 bytes

3. **SAUCE Metadata**: Properly read from files for automatic width/height detection

4. **Line Wrapping**: Content extends beyond SAUCE-specified width (88 chars vs 80 spec)
   - This may be intentional to preserve original art spacing

## Rendering Comparison

### UTF-8 ANSI Terminal Output (`ansilove-utf8ansi`)
- ✓ Preserves CP437 graphics characters with Unicode equivalents
- ✓ Uses 24-bit RGB ANSI SGR codes for color
- ✓ Text-based output suitable for terminal display
- ✓ Small file size (typically 5-10% of PNG)

### PNG Raster Output (system `ansilove`)
- ✓ Rasterizes characters using font rendering
- ✓ Creates bitmap image suitable for archival/display
- ✓ Fixed font dimensions (8×16 or 9×16 pixels per character)
- ✓ Preserves exact original appearance

## Usage

```bash
# Test UTF-8 ANSI output with test file
./ansilove-utf8ansi test_all_chars.ans

# Compare with PNG output
ansilove test_all_chars.ans -o test_all_chars.png
```
