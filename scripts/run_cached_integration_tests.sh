#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "[TaskFlow] Running cached repository integration tests"
ctest --test-dir "${PROJECT_ROOT}/build/Debug" --outpur-on-failure -R "CachedTaskRepositoryTest.*"
