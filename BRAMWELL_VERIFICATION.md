# Bramwell Visual Verification Protocol

**Date:** 2025-10-26  
**Task:** Human visual inspection to validate automated confidence analysis  
**Goal:** Determine if high-confidence samples actually render correctly, low-confidence samples show visible bugs

## Background

Automated analysis of 131 ANSI files identified:
- **Root cause hypothesis:** CR-LF-ESC[A sequences inflate `max_row`, causing extra output lines
- **Confidence scoring:** Based on line delta, SAUCE match, CR-LF-CursorUp frequency, height ratio
- **High confidence samples:** 0 line delta, no CR-LF-UP patterns, match SAUCE perfectly
- **Low confidence samples:** 365-385 line delta, 169-243 CR-LF-UP sequences, 4-5x height inflation

## Files to Verify

### HIGH CONFIDENCE (Expected: Perfect rendering)

**Command to view in terminal:**
```bash
cd /home/tom/Work/libansilove
./build/ansilove-utf8ansi corpus/1996/acid-51a/W7-PHAR1.ANS
./build/ansilove-utf8ansi corpus/1996/fire0296/GK-OLS1.ANS
./build/ansilove-utf8ansi corpus/1996/fire0296/NG-TR1.ANS
./build/ansilove-utf8ansi corpus/1996/fire0296/PN-FONT2.ANS
./build/ansilove-utf8ansi corpus/1996/fire0496/GK-DDL1.ANS
```

**Reference PNG (ground truth):**
```bash
# Compare against system ansilove CLI output
ansilove corpus/1996/acid-51a/W7-PHAR1.ANS -o /tmp/ref.png
# View /tmp/ref.png in image viewer
```

**What to check:**
- Does terminal render look visually complete?
- Are colors correct (DOS palette)?
- Do box-drawing characters connect properly?
- Any obvious glitches, truncation, or corruption?

### LOW CONFIDENCE (Expected: Bug manifestation)

**Command to view:**
```bash
./build/ansilove-utf8ansi corpus/1996/acid-50a/BS-ROCK1.ANS
./build/ansilove-utf8ansi corpus/1996/fire0696/AD-OLIG.ANS
./build/ansilove-utf8ansi corpus/1996/acid-50a/SE-LIME.ANS
./build/ansilove-utf8ansi corpus/1996/fire0496/BV-FREE1.ANS
./build/ansilove-utf8ansi corpus/1996/acid-50a/US-GUBM1.ANS
```

**Reference comparison:**
```bash
ansilove corpus/1996/acid-50a/BS-ROCK1.ANS -o /tmp/ref-bs-rock1.png
# Expected: 135 lines (SAUCE) → 640x2144 px
# Our output: 499 lines
# Hypothesis: Art repeats/overlaps itself due to cursor-up bug
```

**What to check:**
- Does the art repeat vertically (same content drawn multiple times)?
- Are there visible "ghost layers" where text overlaps?
- Does scrolling through show obvious duplication patterns?
- Compare terminal scroll height to reference PNG - does it feel ~3-4x taller?

## Verification Questions

For each sample, answer:

1. **Visual quality (1-5):** How does it look in terminal vs reference PNG?
   - 5 = Indistinguishable
   - 3 = Recognizable but noticeable issues
   - 1 = Severely corrupted

2. **Specific observations:**
   - Duplication? (yes/no + description)
   - Color issues? (yes/no + description)
   - Box drawing broken? (yes/no)
   - Text legible? (yes/no)

3. **Subjective confidence:**
   - Do you agree with automated confidence score?
   - Any issues the automated analysis missed?

## Testing Process

1. Open Alacritty or similar true-color terminal
2. Run each command, observe output
3. Use `Shift+PgUp`/`PgDn` to scroll through full output
4. Open reference PNG in separate window for side-by-side comparison
5. Record observations

## Example Report Format

```
File: W7-PHAR1.ANS (HIGH CONFIDENCE)
Terminal render quality: 5/5
Observations:
  - Colors match DOS palette perfectly
  - Box drawing clean
  - No visible duplication
  - 226 lines feels correct for content density
Automated confidence: VALIDATED ✓

File: BS-ROCK1.ANS (LOW CONFIDENCE)  
Terminal render quality: 2/5
Observations:
  - Art clearly repeats 3-4 times vertically
  - Same logo appears at lines 50, 150, 250, 350
  - Looks like each drawing pass creates a new copy
  - Colors correct, but structure corrupted by over-drawing
Automated confidence: VALIDATED ✓
Bug confirmed: CR-LF-CursorUp causes vertical duplication
```

## Output Location

Save findings to: `/home/tom/Work/libansilove/out/metrics/bramwell_visual_inspection.txt`

## Time Estimate
- 5 high-confidence samples: ~10 minutes (2 min each)
- 5 low-confidence samples: ~15 minutes (3 min each - need comparison)
- **Total: ~25 minutes**

## Success Criteria

After your inspection, we should be able to answer:
1. Does high confidence score = actually good rendering?
2. Does low confidence score = visible duplication bug?
3. Are there edge cases the automated analysis missed?
4. What's the #1 visual artifact to fix first?
