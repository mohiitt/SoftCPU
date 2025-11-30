#!/usr/bin/env bash
set -euo pipefail

PROG=$1
OUT=$2

baseName=$(basename "$PROG" .asm)
PROG_BIN="build/${baseName}.bin"
MAP_FILE="build/${baseName}.map.json"

./bin/software-cpu assemble ${PROG} ${PROG_BIN} ${MAP_FILE}
#./bin/software-cpu run build/${baseName}.bin

current_timestamp=$(date +"%Y%m%d_%H%M%S")

jsonBaseName=$(basename "$OUT" .json)
dirName=$(dirname "$OUT")

goodName="build/traces/${jsonBaseName}_$current_timestamp.json"
mapName="build/traces/${jsonBaseName}_$current_timestamp.map.json"

mkdir -p build/traces
cp ${MAP_FILE} ${mapName}

echo "Generating trace to $goodName for ${PROG_BIN}"
./bin/software-cpu run-trace "${PROG_BIN}" "$goodName"
echo "Trace written to $goodName"
cat "$goodName" | jq '.'