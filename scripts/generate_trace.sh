#!/usr/bin/env bash
set -euo pipefail
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <out.json> <program.bin>"
  exit 2
fi
PROG=$1
OUT=$2
current_timestamp=$(date +"%Y%m%d_%H%M%S")

baseName=$(basename "$OUT" .json)
dirName=$(dirname "$OUT")

goodName="${dirName}/${baseName}_$current_timestamp.json"

mkdir -p build/traces
echo "Generating trace to $goodName for $PROG"
./bin/software-cpu run-trace "$goodName" "$PROG"
echo "Trace written to $goodName"
cat "$goodName" | jq '.'

# # Auto-open the trace viewer with the trace file
# SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
# VIEWER_PATH="$PROJECT_ROOT/trace_viewer/index.html"
# TRACE_ABS="$(cd "$(dirname "$goodName")" && pwd)/$(basename "$goodName")"

# if command -v xdg-open &> /dev/null; then
#   # Linux
#   xdg-open "file://$VIEWER_PATH?trace=$TRACE_ABS"
# elif command -v open &> /dev/null; then
#   # macOS
#   open "file://$VIEWER_PATH?trace=$TRACE_ABS"
# elif command -v start &> /dev/null; then
#   # Windows (Git Bash, MSYS2)
#   start "file://$VIEWER_PATH?trace=$TRACE_ABS"
# else
#   echo "Could not auto-open browser. Open manually:"
#   echo "  $VIEWER_PATH"
#   echo "Then load the trace file: $TRACE_ABS"
# fi
