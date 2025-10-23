# Task: Fix SGR Color Parsing

**Status**: BLOCKED  
**Blockers**: task-tests

## Problem
All cells showing fg=7, bg=0. SGR sequences like `ESC[0;1;40;30m` not updating foreground/background colors correctly.

Root cause: Line 333 in terminal.c adds terminating character ('m', 'H', etc.) to seqGrab, causing strtok to produce tokens like "30m" which strtonum() fails to parse.

## Solution
Remove line 333: `seqGrab[ansi_sequence_loop] = character;`

The terminating character (m, H, f, etc.) should:
- Set `ansi_sequence_character` variable
- NOT be included in the parameter string for tokenization

## Testing
```bash
# Debug test showing color values
./test_colors /home/tom/Downloads/fire-43/US-JELLY.ANS 2>&1 | grep "CELL"
# Should show varying fg/bg values, not all fg=7 bg=0

# First sequence ESC[0;1;40;30m should produce:
# fg=0 (black), bg=0 (black), bold=1
```

## Definition of Done
- [ ] Cells have correct color values from ANSI sequences
- [ ] First 10 cells of US-JELLY.ANS show fg/bg variation
- [ ] strtonum("30m") bug eliminated
- [ ] All SGR parameters parsed correctly (reset, bold, colors)
