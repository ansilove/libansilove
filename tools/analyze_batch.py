#!/usr/bin/env python3
import csv
import sys

def analyze_csv(csv_path):
    with open(csv_path) as f:
        reader = csv.DictReader(f)
        rows = list(reader)
    
    print(f"Total samples: {len(rows)}")
    print()
    
    with_sauce = [r for r in rows if r['has_sauce'] == 'yes']
    without_sauce = [r for r in rows if r['has_sauce'] == 'no']
    
    print(f"With SAUCE: {len(with_sauce)}")
    print(f"Without SAUCE: {len(without_sauce)}")
    print()
    
    rows_numeric = []
    for r in rows:
        try:
            rows_numeric.append({
                'filename': r['filename'],
                'has_sauce': r['has_sauce'],
                'sauce_height': int(r['sauce_height']) if r['sauce_height'] else None,
                'ref_lines': int(r['ref_lines']),
                'utf8_lines': int(r['utf8_lines']),
                'line_delta': int(r['line_delta']),
                'height_ratio': float(r['height_ratio']),
                'size_ratio': float(r['size_ratio'])
            })
        except (ValueError, KeyError):
            continue
    
    rows_numeric.sort(key=lambda x: x['line_delta'])
    
    print("="*80)
    print("BEST MATCHES (smallest line delta)")
    print("="*80)
    for r in rows_numeric[:10]:
        sauce_str = f"SAUCE:{r['sauce_height']}" if r['sauce_height'] else "NO SAUCE"
        print(f"{r['filename']:50s} | {sauce_str:12s} | Δ={r['line_delta']:4d} | ref={r['ref_lines']:3d} utf8={r['utf8_lines']:3d} | ratio={r['height_ratio']:.2f}x")
    
    print()
    print("="*80)
    print("WORST MATCHES (largest line delta)")
    print("="*80)
    for r in rows_numeric[-10:]:
        sauce_str = f"SAUCE:{r['sauce_height']}" if r['sauce_height'] else "NO SAUCE"
        print(f"{r['filename']:50s} | {sauce_str:12s} | Δ={r['line_delta']:4d} | ref={r['ref_lines']:3d} utf8={r['utf8_lines']:3d} | ratio={r['height_ratio']:.2f}x")
    
    print()
    print("="*80)
    print("STATISTICS")
    print("="*80)
    
    deltas = [r['line_delta'] for r in rows_numeric]
    ratios = [r['height_ratio'] for r in rows_numeric]
    
    print(f"Line Delta - Min: {min(deltas)}, Max: {max(deltas)}, Avg: {sum(deltas)/len(deltas):.1f}")
    print(f"Height Ratio - Min: {min(ratios):.2f}x, Max: {max(ratios):.2f}x, Avg: {sum(ratios)/len(ratios):.2f}x")
    
    sauce_rows = [r for r in rows_numeric if r['sauce_height'] is not None]
    if sauce_rows:
        sauce_deltas = [r['line_delta'] for r in sauce_rows]
        nosau_deltas = [r['line_delta'] for r in rows_numeric if r['sauce_height'] is None]
        print()
        print(f"With SAUCE - Avg Delta: {sum(sauce_deltas)/len(sauce_deltas):.1f}")
        print(f"No SAUCE - Avg Delta: {sum(nosau_deltas)/len(nosau_deltas):.1f}")
    
    print()
    print("="*80)
    print("SAUCE HEIGHT vs ACTUAL LINE DELTA (files with SAUCE)")
    print("="*80)
    sauce_matches = []
    for r in sauce_rows:
        expected = r['sauce_height']
        actual = r['ref_lines']
        delta = abs(expected - actual)
        sauce_matches.append((r['filename'], expected, actual, delta, r['line_delta']))
    
    sauce_matches.sort(key=lambda x: x[4])
    
    print(f"{'Filename':50s} | {'SAUCE':>5s} | {'Ref':>5s} | {'ΔSauce':>7s} | {'UTF8Δ':>6s}")
    print("-" * 80)
    for name, sauce, ref, sauce_delta, utf8_delta in sauce_matches[:15]:
        print(f"{name:50s} | {sauce:5d} | {ref:5d} | {sauce_delta:7d} | {utf8_delta:6d}")

if __name__ == '__main__':
    analyze_csv(sys.argv[1] if len(sys.argv) > 1 else 'out/metrics/batch_results.csv')
