# Task: Verify State Machine Fix

**Status**: UNBLOCKED

## Background
Fixed critical bug where STATE_END was 2, same value used for ANSI sequence parameter parsing. This caused parser to exit immediately.

## Change Made
```c
// Before:
#define STATE_TEXT          0
#define STATE_SEQUENCE      1
#define STATE_END           2

// After:
#define STATE_TEXT          0
#define STATE_SEQUENCE      1
#define STATE_SEQUENCE_PARAM 2
#define STATE_END           3
```

## Verification
Ensure this fix is present in src/terminal.c and all tests pass.

## Definition of Done
- [ ] STATE_END = 3 (not 2)
- [ ] STATE_SEQUENCE_PARAM = 2
- [ ] Parser processes full file (not just first 2 bytes)
- [ ] No regression in cursor positioning sequences
