#!/usr/bin/env bash
set -euo pipefail

if ! command -v emcmake >/dev/null; then
  echo "error: emcmake is required (enter via 'nix develop')." >&2
  exit 2
fi

build_dir="${1:-build-wasm}"
artifacts_dir="$build_dir/wasm"
rm -rf "$build_dir"

emcmake cmake -S . -B "$build_dir" -DANSILOVE_BUILD_WASM=ON
cmake --build "$build_dir" --target ansilove_wasm_browser ansilove_wasm_node

browser_js="$artifacts_dir/libansilove.browser.js"
browser_wasm="$artifacts_dir/libansilove.browser.wasm"
node_js="$artifacts_dir/libansilove.node.cjs"
node_wasm="$artifacts_dir/libansilove.node.wasm"

if [ ! -f "$browser_js" ] || [ ! -f "$browser_wasm" ]; then
  echo "error: expected browser artefacts ($browser_js, $browser_wasm)" >&2
  exit 3
fi

if [ ! -f "$node_js" ] || [ ! -f "$node_wasm" ]; then
  echo "error: expected node artefacts ($node_js, $node_wasm)" >&2
  exit 4
fi

node scripts/test-wasm-node.js "$artifacts_dir" "$artifacts_dir/test-output.png" "$node_js"

cp "$browser_js" example/wasm/libansilove.browser.js
cp "$browser_wasm" example/wasm/libansilove.browser.wasm

echo "WASM artefacts found in $artifacts_dir" >&2
