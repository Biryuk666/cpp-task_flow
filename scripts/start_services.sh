#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "[TaskFlow] Starting PostgreSQL and Redis"
docker compose -f "${PROJECT_ROOT}/docker-compose.yml" up -d --wait

echo "[TaskFlow] Services started"
docker compose -f "${PROJECT_ROOT}/docker-compose.yml" ps
