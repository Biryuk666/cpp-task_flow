#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "[TaskFlow] Stopping services"
docker compose -f "${PROJECT_ROOT}/docker-compose.yml" down
