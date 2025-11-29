#!/usr/bin/env bash
set -euo pipefail
echo "Building project..."
make
echo "Build complete. Binaries in bin/"
