#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

"${PROJECT_ROOT}/scripts/conan_install_debug.sh"
"${PROJECT_ROOT}/scripts/build_debug.sh"
"${PROJECT_ROOT}/scripts/run_tests.sh"
