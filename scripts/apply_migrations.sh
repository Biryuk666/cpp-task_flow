#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ -f "${PROJECT_ROOT}/.env" ]]; then
  set -a
  source "${PROJECT_ROOT}/.env"
  set +a
fi

SQL_FILE="${PROJECT_ROOT}/db/migrations/001_init.sql"

if [[ ! -f "${SQL_FILE}" ]]; then
  echo "[TaskFlow] Migration file not found: ${SQL_FILE}"
  exit 1
fi

echo "[TaskFlow] Applying migration: ${SQL_FILE}"

export PGPASSWORD="${POSTGRES_PASSWORD}"

psql \
  -h "${POSTGRES_HOST}" \
  -p "${POSTGRES_PORT}" \
  -U "${POSTGRES_USER}" \
  -d "${POSTGRES_DB}" \
  -f "${SQL_FILE}"

echo "[TaskFlow] Migration applied successfully"
