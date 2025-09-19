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
cmake --build "$build_dir" --target ansilove_wasm

if [ ! -f "$artifacts_dir/libansilove.wasm" ]; then
  echo "error: expected $artifacts_dir/libansilove.wasm to exist" >&2
  exit 3
fi

if [ ! -f "$artifacts_dir/libansilove.js" ]; then
  echo "error: expected $artifacts_dir/libansilove.js to exist" >&2
  exit 4
fi

node scripts/test-wasm-node.js "$artifacts_dir" "$artifacts_dir/test-output.png"

cp "$artifacts_dir/libansilove.js" example/wasm/
cp "$artifacts_dir/libansilove.wasm" example/wasm/

echo "WASM artefacts found in $artifacts_dir" >&2
