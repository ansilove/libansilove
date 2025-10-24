# UTF-8 + ANSI Terminal Output

## Quick Start
```bash
# Build
gcc -o ansi_viewer viewer.c src/terminal.c src/loadfile.c src/init.c src/error.c \
    src/clean_minimal.c compat/strtonum.c compat/reallocarray.c \
    -Iinclude -Isrc -Icompat -lm -D_GNU_SOURCE

# Use
./ansi_viewer artwork.ans | less -R
```

## What Works
✅ DOS ANSI → UTF-8 + ANSI SGR conversion  
✅ CP437 character encoding (blocks, box-drawing, etc.)  
✅ DOS color palette → ANSI 256-color codes  
✅ Bold, blink attributes  
✅ Large files (tested on 162KB US-JELLY.ANS)  

## Key Fixes Applied
1. **State machine bug**: STATE_END collision with sequence parsing state
2. **Color parsing bug**: Removed terminating character from SGR parameter string
3. **CP437 table**: Corrected Unicode mappings for block characters

## Testing
Verified against cat-ans reference implementation on fire-43 ANSI art collection (13 files).
