#!/usr/bin/env python3
import csv
import sys
import subprocess
import os

def analyze_confidence(csv_path, corpus_root="corpus/1996"):
    with open(csv_path) as f:
        reader = csv.DictReader(f)
        rows = list(reader)
    
    samples = []
    for r in rows:
        try:
            filename = r['filename']
            has_sauce = r['has_sauce'] == 'yes'
            sauce_height = int(r['sauce_height']) if r['sauce_height'] else None
            ref_lines = int(r['ref_lines'])
            utf8_lines = int(r['utf8_lines'])
            line_delta = int(r['line_delta'])
            height_ratio = float(r['height_ratio'])
            
            ansi_path = None
            for pack in os.listdir(corpus_root):
                pack_path = os.path.join(corpus_root, pack)
                if os.path.isdir(pack_path):
                    for ans_file in os.listdir(pack_path):
                        if ans_file.endswith('.ANS'):
                            test_name = f"{pack}_{ans_file[:-4]}"
                            if test_name == filename:
                                ansi_path = os.path.join(pack_path, ans_file)
                                break
                if ansi_path:
                    break
            
            if not ansi_path:
                continue
            
            result = subprocess.run(
                ['hexdump', '-C', ansi_path],
                capture_output=True,
                text=True
            )
            cr_lf_esc_a = result.stdout.count('0d 0a 1b 5b 41')
            
            confidence_score = 0.0
            factors = []
            
            if line_delta == 0:
                confidence_score += 50
                factors.append("perfect_line_match")
            elif line_delta < 10:
                confidence_score += 40
                factors.append("near_perfect_match")
            elif line_delta < 50:
                confidence_score += 20
            else:
                confidence_score -= (line_delta / 10)
            
            if has_sauce and sauce_height == utf8_lines:
                confidence_score += 30
                factors.append("matches_sauce")
            elif has_sauce and sauce_height == ref_lines:
                confidence_score += 20
                factors.append("ref_matches_sauce")
            
            if cr_lf_esc_a == 0:
                confidence_score += 20
                factors.append("no_cr_lf_cursor_up")
            elif cr_lf_esc_a > 100:
                confidence_score -= 20
                factors.append(f"heavy_cursor_up_{cr_lf_esc_a}")
            
            if height_ratio < 1.5:
                confidence_score += 10
                factors.append("good_ratio")
            elif height_ratio > 4.0:
                confidence_score -= 15
                factors.append("bad_ratio")
            
            samples.append({
                'filename': filename,
                'ansi_path': ansi_path,
                'confidence': confidence_score,
                'factors': factors,
                'line_delta': line_delta,
                'cr_lf_esc_a': cr_lf_esc_a,
                'has_sauce': has_sauce,
                'sauce_height': sauce_height,
                'ref_lines': ref_lines,
                'utf8_lines': utf8_lines,
                'height_ratio': height_ratio
            })
        except (ValueError, KeyError) as e:
            continue
    
    samples.sort(key=lambda x: x['confidence'], reverse=True)
    
    print("="*100)
    print("CONFIDENCE ANALYSIS - UTF8ANSI Terminal Rendering")
    print("="*100)
    print()
    
    print("HIGHEST CONFIDENCE (likely correct rendering):")
    print("-"*100)
    for s in samples[:5]:
        print(f"{s['filename']:50s} | Score: {s['confidence']:6.1f} | Δ={s['line_delta']:3d} | CR-LF-UP={s['cr_lf_esc_a']:3d}")
        print(f"  Factors: {', '.join(s['factors'])}")
        print(f"  Path: {s['ansi_path']}")
        print()
    
    print()
    print("LOWEST CONFIDENCE (likely rendering issues):")
    print("-"*100)
    for s in samples[-5:]:
        print(f"{s['filename']:50s} | Score: {s['confidence']:6.1f} | Δ={s['line_delta']:3d} | CR-LF-UP={s['cr_lf_esc_a']:3d}")
        print(f"  Factors: {', '.join(s['factors'])}")
        print(f"  Path: {s['ansi_path']}")
        print()
    
    return samples

if __name__ == '__main__':
    samples = analyze_confidence('out/metrics/batch_results.csv')
    
    with open('out/metrics/confidence_ranking.txt', 'w') as f:
        f.write("Confidence Ranking (High to Low)\n")
        f.write("="*100 + "\n")
        for i, s in enumerate(samples, 1):
            f.write(f"{i:3d}. {s['filename']:50s} | Score: {s['confidence']:6.1f} | Δ={s['line_delta']:3d}\n")
    
    print()
    print(f"Full ranking saved to: out/metrics/confidence_ranking.txt")
