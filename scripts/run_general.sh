#!/usr/bin/env bash
set -euo pipefail

PROG=$1
baseName=$(basename "$PROG" .asm)

./bin/software-cpu assemble ${PROG} build/${baseName}.bin
./bin/software-cpu run build/${baseName}.bin
