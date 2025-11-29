#!/usr/bin/env bash
set -euo pipefail
./bin/software-cpu assemble src/programs/fibonacci.asm build/fibonacci.bin
./bin/software-cpu run build/fibonacci.bin
