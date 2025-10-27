# Human Visual Review Instructions for Bramwell

## Context
We've run automated batch comparison across 131 ANSI art files from the 1996 corpus, comparing:
- **Reference**: System `ansilove` CLI (AnsiLove/C 4.2.1) PNG output
- **Test**: `./ansilove-utf8ansi` → `ansee` PNG pipeline

Automated metrics identified 10 best matches (0 line delta) and 10 worst matches (344-396 line delta). Your task is to visually inspect these samples and provide subjective feedback on what automated metrics miss.

## Files to Review

### Best Matches (0 line delta - perfect line count)
Located in: `out/reference/` vs `out/current/`

1. `acid-51a_W7-PHAR1.png` (226 lines, SAUCE: yes)
2. `fire0296_GK-OLS1.png` (39 lines, SAUCE: yes)
3. `fire0296_NG-TR1.png` (54 lines, SAUCE: yes)
4. `fire0296_PN-FONT2.png` (337 lines, SAUCE: yes)

### Worst Matches (344-396 line delta)
1. `acid-51a_RD-MOOSE.png` (Δ=396, ref=103 vs utf8=499, ratio=6.66x)
2. `acid-50a_US-GUBM1.png` (Δ=385, ref=114 vs utf8=499, ratio=6.02x)
3. `acid-51a_SE-DMCRG.png` (Δ=381, ref=118 vs utf8=499, ratio=5.81x)
4. `fire0496_BV-FREE1.png` (Δ=377, ref=122 vs utf8=499, ratio=5.62x)

### Interesting Edge Cases
5. `acid-50a_NEWS-50.png` (NO SAUCE, Δ=56, ref=443 vs utf8=499)
6. `fire0296_33-TSP1.png` (SAUCE=25 but ref=100, Δ=0) ← SAUCE mismatch but perfect terminal output

## Review Protocol

For each pair of images, open them side-by-side and assess:

### 1. Visual Fidelity
- **Color accuracy**: Do DOS palette colors match? (ignore anti-aliasing differences)
- **Character placement**: Are glyphs in the correct positions?
- **Box drawing**: Do line-draw characters connect properly?
- **Text legibility**: Is ASCII text readable in both renders?

### 2. Structural Issues
- **Extra whitespace**: Does utf8ansi+ansee version show excessive blank rows at top/bottom?
- **Content truncation**: Is any art content missing or cut off?
- **Aspect ratio**: Does the art appear stretched or compressed?
- **Column alignment**: Do vertical elements align properly?

### 3. Rendering Artifacts
- **Font differences**: Note bitmap (reference) vs TrueType (ansee) rendering quality
- **Anti-aliasing**: ansee uses sub-pixel rendering - does it improve or degrade readability?
- **Color bleeding**: Any unwanted color halos or transparency issues?

### 4. Subjective Assessment
Rate each pair on a scale of 1-5:
- **5**: Visually indistinguishable (accounting for font differences)
- **4**: Minor differences, art intent preserved
- **3**: Noticeable differences, art still recognizable
- **2**: Significant distortion, art partially degraded
- **1**: Severe corruption, art unrecognizable

## Reporting Format

For each reviewed file, provide:

```
Filename: acid-51a_W7-PHAR1
Rating: 4/5
Observations:
- Colors match DOS palette accurately
- Extra 273 blank rows at bottom (padding issue)
- Box drawing characters render cleanly
- TrueType font actually improves small text legibility
- Art content intact, no truncation

Verdict: Acceptable with known padding bug
```

## Key Questions to Answer

1. **Do the "best matches" (0 line delta) actually look visually superior?**
   - Hypothesis: Low delta doesn't guarantee visual quality if colors/glyphs are wrong

2. **What makes the "worst matches" bad?**
   - Is it just extra padding (expected bug) or actual content corruption?
   - Can you identify specific ANSI sequences that cause problems?

3. **SAUCE anomalies**:
   - Files like `fire0296_33-TSP1` have SAUCE=25 but render 100 lines
   - Does the visual output match SAUCE metadata or actual content?

4. **Font rendering preference**:
   - Do you prefer bitmap (sharp, pixelated) or TrueType (smooth, anti-aliased)?
   - Which is more faithful to 1990s BBS aesthetics?

## Expected Findings

Based on automated analysis, we expect:
- **Best matches**: Line count perfect, but may still have color/glyph issues
- **Worst matches**: Excessive bottom padding (known terminal.c bug), but art content likely intact
- **SAUCE mismatches**: Metadata errors in original files, not rendering bugs

Your visual inspection will validate or contradict these automated conclusions.

## Submission

Save findings to: `out/metrics/human_review_bramwell.txt`

Include:
- Individual file ratings
- Summary observations
- Recommended samples for bug reproduction
- Any patterns noticed across files (e.g., "all Fire packs have X issue")

## Time Estimate
- ~15 minutes for 10 file pairs (90 seconds per pair)
- Prioritize extremes (best/worst) over middle-ground samples
