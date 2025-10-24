#!/bin/bash
#
# Demo script for libansilove UTF-8+ANSI terminal mode
# Shows peer reviewers how to convert DOS ANSI art for Linux terminals
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANSILOVE_UTF8ANSI="$SCRIPT_DIR/ansilove-utf8ansi"

if [ ! -x "$ANSILOVE_UTF8ANSI" ]; then
    echo "Error: ansilove-utf8ansi not found or not executable"
    echo "Expected: $ANSILOVE_UTF8ANSI"
    exit 1
fi

echo "============================================"
echo "libansilove UTF-8+ANSI Terminal Mode Demo"
echo "============================================"
echo

# Example 1: Print to terminal
echo "Example 1: Print DOS ANSI to Linux terminal"
echo "-------------------------------------------"
echo "Command: ansilove-utf8ansi <file.ans>"
echo

if [ -f "$1" ]; then
    INPUT_FILE="$1"
else
    # Use default test file if provided
    INPUT_FILE="$SCRIPT_DIR/ansi_test_files/simple_colors.ans"
    if [ ! -f "$INPUT_FILE" ]; then
        echo "Note: No test file found. Usage: $0 <file.ans>"
        echo "      Using stdin example instead..."
        echo
        echo -e "\033[0m\033[31m▄▄▄▄▄\033[0m \033[32m▄▄▄▄▄\033[0m \033[34m▄▄▄▄▄\033[0m"
        echo
        exit 0
    fi
fi

echo "Input: $INPUT_FILE"
echo
echo "Output:"
"$ANSILOVE_UTF8ANSI" "$INPUT_FILE" 2>/dev/null || echo "(File not displayable)"
echo

# Example 2: Save to file
echo
echo "Example 2: Save as .utf8ansi file"
echo "----------------------------------"
echo "Command: ansilove-utf8ansi <file.ans> > <file.utf8ansi>"
echo

OUTPUT_FILE="/tmp/demo.utf8ansi"
"$ANSILOVE_UTF8ANSI" "$INPUT_FILE" > "$OUTPUT_FILE" 2>/dev/null

if [ -f "$OUTPUT_FILE" ]; then
    SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)
    LINES=$(wc -l < "$OUTPUT_FILE")
    echo "Created: $OUTPUT_FILE ($SIZE, $LINES lines)"
    echo
    
    # Example 3: Use with ansee (if available)
    if command -v ansee &> /dev/null; then
        echo
        echo "Example 3: Render .utf8ansi to PNG with ansee"
        echo "----------------------------------------------"
        echo "Command: ansee <file.utf8ansi> -o <file.png>"
        echo
        
        PNG_FILE="/tmp/demo.png"
        if ansee "$OUTPUT_FILE" -o "$PNG_FILE" 2>/dev/null; then
            PNG_SIZE=$(du -h "$PNG_FILE" | cut -f1)
            echo "Created: $PNG_FILE ($PNG_SIZE)"
            echo "Note: ansee uses TrueType rendering with anti-aliasing"
        else
            echo "Note: ansee may skip some SGR codes (bold, blink)"
            echo "      PNG still created, but may differ from terminal view"
        fi
    else
        echo
        echo "Note: ansee not found in PATH"
        echo "      Install from: https://github.com/ansi-art/ansee"
        echo "      Or use: cat <file.utf8ansi> to view in terminal"
    fi
fi

echo
echo "============================================"
echo "Format Details"
echo "============================================"
echo "Input:  DOS ANSI (.ans) - CP437 + DOS color codes"
echo "Output: Linux ANSI (.utf8ansi) - UTF-8 + ANSI 256-color SGR"
echo
echo "Features:"
echo "  ✓ CP437 → Unicode character conversion"
echo "  ✓ DOS palette → ANSI 256-color mapping"
echo "  ✓ Full SGR support (bold, blink, invert)"
echo "  ✓ Direct terminal display"
echo "  ✓ Piping support (stdout)"
echo
echo "Usage Examples:"
echo "  ansilove-utf8ansi file.ans                    # Display in terminal"
echo "  ansilove-utf8ansi file.ans > file.utf8ansi    # Save to file"
echo "  ansilove-utf8ansi file.ans | less -R          # Page through with color"
echo "  cat file.utf8ansi                             # Display saved file"
echo "  ansee file.utf8ansi -o file.png               # Render to PNG"
echo
