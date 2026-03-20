#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "[TaskFlow] Configuring project"
cmake --preset conan-debug

echo "[TaskFlow] Building project"
cmake --build --preset build-debug
