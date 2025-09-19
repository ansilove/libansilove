#!/usr/bin/env bash
set -euo pipefail

if ! command -v bun >/dev/null; then
  echo "error: bun is required (enter via 'nix develop')." >&2
  exit 2
fi

artifacts_dir="npm/packages/libansilove/generated"
browser_js="$artifacts_dir/libansilove.browser.mjs"
browser_wasm="$artifacts_dir/libansilove.browser.wasm"
node_js="$artifacts_dir/libansilove.node.mjs"
node_wasm="$artifacts_dir/libansilove.node.wasm"

bun npm/packages/libansilove/scripts/build-wasm.bun.ts

if [ ! -f "$browser_js" ] || [ ! -f "$browser_wasm" ]; then
  echo "error: expected browser artefacts ($browser_js, $browser_wasm)" >&2
  exit 3
fi

if [ ! -f "$node_js" ] || [ ! -f "$node_wasm" ]; then
  echo "error: expected node artefacts ($node_js, $node_wasm)" >&2
  exit 4
fi

node scripts/test-wasm-node.js "$artifacts_dir" "$artifacts_dir/test-output.png" "$node_js"

mkdir -p example/wasm
cp "$browser_js" example/wasm/libansilove.browser.mjs
cp "$browser_wasm" example/wasm/libansilove.browser.wasm

echo "WASM artefacts found in $artifacts_dir" >&2
