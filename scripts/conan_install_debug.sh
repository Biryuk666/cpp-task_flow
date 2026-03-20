#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/Debug/generators"

echo "[TaskFlow] Installing Conan dependencies into ${BUILD_DIR}"

conan install "${PROJECT_ROOT}" \
    --build=missing \
    -s build_type=Debug
