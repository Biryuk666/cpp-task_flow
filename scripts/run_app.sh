#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_PATH="${PROJECT_ROOT}/build/Debug/src/TaskFlow_app"

if [[ ! -x "${APP_PATH}" ]]; then
echo "[TaskFlow] App not found: "${APP_PATH}""
echo "[TaskFlow] Build the project first: ./scripts/dev_build.sh"
exit 1
fi

echo "[taskflow] Running app"
"${APP_PATH}"
