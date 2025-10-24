# UTF-8 + ANSI Terminal Output for libansilove

## Goal
Add a terminal output mode to libansilove that converts DOS ANSI art files to UTF-8 text with ANSI SGR color codes, suitable for display in modern Linux terminals.

## Success Criteria
```bash
# Works with any DOS ANSI file
./ansi_viewer /path/to/artwork.ans | less -R

# Shows correct:
# - Block characters (CP437 → Unicode)
# - Colors (DOS palette → ANSI256 or RGB)
# - Box drawing characters
# - All special glyphs
```

## Scope
- **In scope**: UTF-8 encoding, ANSI SGR colors, DOS→Unicode mapping, terminal mode backend
- **Out of scope**: Image rendering, font handling, GUI, animated sequences

## Technical Approach
1. Parse DOS ANSI escape sequences (already exists in codebase)
2. Accumulate characters in a grid with color attributes
3. Convert CP437 characters to UTF-8
4. Map DOS colors to ANSI 256-color palette
5. Emit UTF-8 text with ANSI SGR codes

## Current Status
- Terminal backend exists (`src/terminal.c`) but has bugs
- CP437→Unicode table exists but had errors (now fixed)
- Color mapping incomplete/broken
- State machine had critical bug (STATE_END collision)

## Known Issues
- Color parsing broken: all cells showing same color (fg=7, bg=0)
- SGR sequence tokenization includes trailing 'm' character
- No comprehensive tests against reference output

## Reference Implementation
`cat-ans` (Python tool) produces correct output - use as gold standard for verification.
