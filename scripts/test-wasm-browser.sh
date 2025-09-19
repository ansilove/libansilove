#!/usr/bin/env bash
set -euo pipefail

PORT="${PORT:-8765}"
DOC_ROOT="example/wasm"
URL="http://localhost:${PORT}/index.html"

if ! command -v osascript >/dev/null; then
  echo "error: osascript is required on macOS to drive Safari." >&2
  exit 2
fi

if ! command -v python3 >/dev/null; then
  echo "error: python3 is required to start the local web server." >&2
  exit 2
fi

if [ ! -f "${DOC_ROOT}/libansilove.browser.mjs" ]; then
  echo "error: ${DOC_ROOT}/libansilove.browser.mjs not found; run scripts/test-wasm.sh first." >&2
  exit 3
fi

LOG_FILE=$(mktemp -t libansilove-wasm-http.XXXXXX.log)
cleanup() {
  if [ -n "${SERVER_PID:-}" ]; then
    kill "${SERVER_PID}" 2>/dev/null || true
  fi
  rm -f "$LOG_FILE"
}
trap cleanup EXIT

python3 -m http.server "${PORT}" --directory "${DOC_ROOT}" >"$LOG_FILE" 2>&1 &
SERVER_PID=$!

# Give the server a moment to come up
sleep 1

"$(dirname "$0")/safari-wasm-smoke.js" "$URL"

echo "Safari smoke test passed for $URL"
