# Task: Create Test Suite

**Status**: UNBLOCKED

## Objective
Build minimal test programs to verify color parsing and character encoding independently.

## Tests Needed

### 1. test_sgr_parse.c
Parse SGR sequences and print extracted values:
```c
// Input: "0;1;40;30"
// Output: reset=1 bold=1 bg=0 fg=0
```

### 2. test_color_output.c  
Read ANSI file, show first N cells with their color values:
```bash
./test_color_output file.ans 10
# Cell[0,0]: ch=0xDB fg=0 bg=0 bold=1
# Cell[0,1]: ch=0xDB fg=0 bg=0 bold=1
```

### 3. test_reference_compare.c
Compare our output vs cat-ans byte-by-byte for first 100 chars.

## Definition of Done
- [ ] All 3 test programs compile
- [ ] test_sgr_parse correctly tokenizes "0;1;40;30" without 'm'
- [ ] test_color_output shows cell attributes
- [ ] Tests documented in .specs/utf8ansi/testing.md
