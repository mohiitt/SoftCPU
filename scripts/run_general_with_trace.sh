#!/usr/bin/env bash
set -euo pipefail

PROG=$1
OUT=$2

baseName=$(basename "$PROG" .asm)
PROG_BIN="build/${baseName}.bin"

./bin/software-cpu assemble ${PROG} ${PROG_BIN}
#./bin/software-cpu run build/${baseName}.bin

current_timestamp=$(date +"%Y%m%d_%H%M%S")

jsonBaseName=$(basename "$OUT" .json)
dirName=$(dirname "$OUT")

goodName="build/traces/${jsonBaseName}_$current_timestamp.json"

mkdir -p build/traces
echo "Generating trace to $goodName for ${PROG_BIN}"
./bin/software-cpu run-trace "${PROG_BIN}" "$goodName"
echo "Trace written to $goodName"
cat "$goodName" | jq '.'