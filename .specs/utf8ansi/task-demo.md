# Task: Final Demo - End-to-End Verification

**Status**: BLOCKED  
**Blockers**: task-color-fix, task-build-clean

## Success Criteria
```bash
# Test on multiple fire-43 ANSI files
for f in /home/tom/Downloads/fire-43/*.ANS; do
    echo "=== $(basename "$f") ==="
    ./ansi_viewer "$f" | head -10
done

# Visual verification:
./ansi_viewer /home/tom/Downloads/fire-43/US-JELLY.ANS | ansee -o /tmp/my_render.png
ansee /home/tom/Downloads/fire-43/US-JELLY.ANS -o /tmp/cat_ans_render.png
# Compare PNGs - should be visually identical

# Bash oneliner works:
cat-ans "$FILE" | less -R  # reference
./ansi_viewer "$FILE" | less -R  # ours (should match colors/chars)
```

## Definition of Done
- [ ] ansi_viewer binary exists and is executable
- [ ] Processes US-JELLY.ANS (162KB) without errors
- [ ] Output contains colored blocks (not all gray)
- [ ] Output visually matches cat-ans reference
- [ ] Works on all 13 fire-43 test files
- [ ] Bash oneliner documented in README

## Verification Method
Generate PNG comparison with ansee, visual inspection of colors and characters.
