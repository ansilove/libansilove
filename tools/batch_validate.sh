#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

CORPUS_LIST="${1:-/tmp/corpus_ansi_files.txt}"
OUTPUT_CSV="out/metrics/batch_results.csv"
ANSEE_PATH="${HOME}/.cargo/bin/ansee"

if [ ! -f "$CORPUS_LIST" ]; then
    echo "Error: Corpus list not found: $CORPUS_LIST"
    exit 1
fi

if [ ! -x "./build/ansilove-utf8ansi" ]; then
    echo "Error: ./build/ansilove-utf8ansi not found or not executable"
    exit 1
fi

if [ ! -x "$ANSEE_PATH" ]; then
    echo "Error: ansee not found at $ANSEE_PATH"
    exit 1
fi

mkdir -p out/reference out/current out/terminal out/metrics

echo "filename,has_sauce,sauce_width,sauce_height,ref_width,ref_height,ref_lines,utf8_lines,utf8_width,utf8_height,line_delta,height_ratio,filesize_ref,filesize_utf8,size_ratio" > "$OUTPUT_CSV"

total=$(wc -l < "$CORPUS_LIST")
count=0

while IFS= read -r ansi_file; do
    count=$((count + 1))
    basename=$(basename "$ansi_file" .ANS)
    dirname=$(dirname "$ansi_file" | sed 's|corpus/1996/||')
    safe_name="${dirname//\//_}_${basename}"
    
    echo "[$count/$total] Processing: $ansi_file"
    
    ref_png="out/reference/${safe_name}.png"
    utf8_file="out/terminal/${safe_name}.utf8ansi"
    utf8_png="out/current/${safe_name}.png"
    
    ansilove "$ansi_file" -o "$ref_png" 2>&1 | grep -E "SAUCE|Tinfo|Columns" > "/tmp/${safe_name}_sauce.txt" || echo "No SAUCE" > "/tmp/${safe_name}_sauce.txt"
    
    has_sauce="no"
    sauce_width=""
    sauce_height=""
    if grep -q "^Id: SAUCE" "/tmp/${safe_name}_sauce.txt" 2>/dev/null; then
        has_sauce="yes"
        sauce_width=$(grep "^Tinfo1:" "/tmp/${safe_name}_sauce.txt" | awk '{print $2}' || echo "")
        sauce_height=$(grep "^Tinfo2:" "/tmp/${safe_name}_sauce.txt" | awk '{print $2}' || echo "")
    fi
    
    ./build/ansilove-utf8ansi "$ansi_file" > "$utf8_file" 2>&1 || {
        echo "ERROR: ansilove-utf8ansi failed on $ansi_file"
        continue
    }
    
    "$ANSEE_PATH" -o "$utf8_png" < "$utf8_file" >/dev/null 2>&1 || {
        echo "WARN: ansee failed on $utf8_file (continuing)"
    }
    
    ref_dims=$(identify -format "%w %h" "$ref_png" 2>/dev/null || echo "0 0")
    ref_width=$(echo "$ref_dims" | awk '{print $1}')
    ref_height=$(echo "$ref_dims" | awk '{print $2}')
    ref_lines=$((ref_height / 16))
    
    utf8_dims=$(identify -format "%w %h" "$utf8_png" 2>/dev/null || echo "0 0")
    utf8_width=$(echo "$utf8_dims" | awk '{print $1}')
    utf8_height=$(echo "$utf8_dims" | awk '{print $2}')
    
    utf8_lines=$(wc -l < "$utf8_file")
    
    line_delta=$((utf8_lines - ref_lines))
    
    if [ "$ref_height" -gt 0 ]; then
        height_ratio=$(awk "BEGIN {printf \"%.2f\", $utf8_height / $ref_height}")
    else
        height_ratio="0"
    fi
    
    ref_size=$(stat -c%s "$ref_png" 2>/dev/null || echo "0")
    utf8_size=$(stat -c%s "$utf8_png" 2>/dev/null || echo "0")
    
    if [ "$ref_size" -gt 0 ]; then
        size_ratio=$(awk "BEGIN {printf \"%.2f\", $utf8_size / $ref_size}")
    else
        size_ratio="0"
    fi
    
    echo "${safe_name},${has_sauce},${sauce_width},${sauce_height},${ref_width},${ref_height},${ref_lines},${utf8_lines},${utf8_width},${utf8_height},${line_delta},${height_ratio},${ref_size},${utf8_size},${size_ratio}" >> "$OUTPUT_CSV"
    
done < "$CORPUS_LIST"

echo ""
echo "Batch processing complete. Results: $OUTPUT_CSV"
echo "Total files processed: $count"
