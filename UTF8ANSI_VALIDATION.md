# UTF8ANSI Terminal Mode Validation

**Experiment Date:** 2025-10-26T09:48:52-04:00  
**Sample File:** `corpus/1996/acid-50a/NEWS-50.ANS`  
**Test Type:** End-to-end comparison of ansilove CLI vs. utf8ansi+ansee pipeline

## Methodology

1. **Reference Render**: Generate PNG using system `ansilove` CLI (AnsiLove/C 4.2.1)
2. **Test Render**: Generate UTF8+ANSI output via `./ansilove-utf8ansi`, pipe to `ansee` for PNG
3. **Determinism Check**: Run pipeline twice, hash terminal output and PNG outputs
4. **Metrics Collection**: Compare dimensions, pixel diffs, palette histograms
5. **Reproducibility**: Document all tool versions and commands

## Tool Versions

- **ansilove CLI**: AnsiLove/C 4.2.1
- **ansee**: (no --version flag; help shows "Render ANSI escaped text to image")
- **ImageMagick**: 7.1.2-7 Q16-HDRI x86_64
- **System**: Linux 6.17.5-arch1-1 x86_64

## Experimental Results

### 1. Determinism (PASS ✓)

**Terminal Output:**
```
md5sum run1: 0eadc9800bfd574578d74cf387de34bd
md5sum run2: 0eadc9800bfd574578d74cf387de34bd
```
**Conclusion:** `./ansilove-utf8ansi` produces identical terminal output across runs.

**PNG Output:**
```
md5sum run1: d1c629f00f1c28b50b3137dcd7824213
md5sum run2: d1c629f00f1c28b50b3137dcd7824213
```
**Conclusion:** `ansee` produces identical PNG output from identical terminal input.

### 2. Dimensional Comparison

| Metric | ansilove CLI | utf8ansi+ansee | Ratio |
|--------|-------------|----------------|-------|
| Width | 640 px | 1604 px | 2.51x |
| Height | 7088 px | 10978 px | 1.55x |
| File Size | 150 KB | 2.59 MB | 17.3x |
| Rendered Lines | 443 | 499 | 1.13x |
| Terminal Lines Output | N/A | 499 | - |

**Analysis:**
- Reference renders 443 lines (7088 px ÷ 16 px/line)
- utf8ansi outputs 499 terminal lines
- ansee renders at ~22 px/line (10978 ÷ 499 = 22 px)
- **56 extra lines** (499 - 443 = 56) due to sparse row output issue identified in terminal.c

### 3. Pixel Difference

```
compare -metric AE: 1,526,080 absolute error pixels
```
**Conclusion:** Pixel-level comparison is invalid due to:
- Different canvas dimensions (640x7088 vs 1604x10978)
- Different font rendering (bitmap vs TrueType with anti-aliasing)

### 4. Palette Analysis

**ansilove CLI (reference):**
- 12 unique colors (exact DOS palette)
- No transparency
- All colors map to standard VGA palette

**utf8ansi+ansee (current):**
- 1217 unique colors
- Includes alpha channel (RGBA)
- Anti-aliased text introduces color gradients
- Transparent background (15.1M transparent pixels)

**Analysis:**
- `ansee` uses TrueType font rendering with anti-aliasing
- Creates gradients between foreground/background (sub-pixel rendering)
- Not comparable to `ansilove`'s pixel-perfect bitmap fonts
- This is expected behavior, not a defect

## Known Issues Identified

### Issue 1: Extra Line Output (High Priority)
**Observation:** utf8ansi outputs 499 lines; file should render as 443 lines  
**Root Cause:** `src/terminal.c:501` outputs all rows from 0 to `grid->max_row`  
**Impact:** 56 extra blank/sparse lines (12.6% overhead)  
**Status:** Documented in earlier analysis; requires SAUCE height enforcement

### Issue 2: No SAUCE Height Enforcement
**Observation:** File has no SAUCE record; line count determined by grid traversal  
**Expected:** Should respect SAUCE height when present, trim trailing blank rows when absent  
**Status:** Feature gap

### Issue 3: Font Rendering Differences (By Design)
**Observation:** ansee anti-aliasing creates 1217 colors vs. ansilove's 12  
**Status:** Intentional design difference; not a bug

## Validation of Methodology

### Strengths
✓ Pipeline is deterministic (identical hashes across runs)  
✓ Tool versions captured for reproducibility  
✓ Multiple metrics collected (dimensions, palette, file size)  
✓ Commands documented for peer review

### Weaknesses Identified
✗ Single sample file (NEWS-50.ANS without SAUCE)  
✗ Pixel diff metrics invalid due to different rendering approaches  
✗ No automated script yet; manual command execution  
✗ ansee version not captured (no --version flag)  
✗ No comparison against files *with* SAUCE records

## Peer Review Considerations

**Reproducibility:**  
All commands can be re-executed from project root:
```bash
ansilove corpus/1996/acid-50a/NEWS-50.ANS -o out/reference/NEWS-50.ANS.png
./ansilove-utf8ansi corpus/1996/acid-50a/NEWS-50.ANS > out/terminal/NEWS-50.utf8ansi
cat out/terminal/NEWS-50.utf8ansi | ansee -o out/current/NEWS-50.utf8ansi.png
```

**Limitations:**  
- Requires `ansee` in PATH (Rust binary from ~/.cargo/bin)
- System `ansilove` must be AnsiLove/C 4.2.1 (results may vary with other versions)
- ImageMagick 7.x required for histogram commands

## Next Steps

1. **Expand Sample Size**: Test 10+ files with varying characteristics (SAUCE/no-SAUCE, different packs)
2. **Fix Line Count Issue**: Implement SAUCE height enforcement in terminal.c
3. **Automate Testing**: Create `tools/validate_utf8ansi.sh` script
4. **Baseline Comparison**: Build and stash current binary before fixes
5. **Alternative Renderer**: Test against second UTF8 renderer to triangulate ansee-specific issues

## Conclusion

**Determinism**: VALIDATED ✓  
**Methodology**: SOUND (with documented limitations)  
**Line Count Accuracy**: FAILED (56 extra lines output)  
**Color Fidelity**: NOT COMPARABLE (different rendering paradigms)  

The validation methodology is reproducible and scientifically sound. The primary actionable finding is the confirmed line-count inflation issue requiring code fixes in `src/terminal.c`.
