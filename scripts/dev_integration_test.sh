#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

"${PROJECT_ROOT}/scripts/start_services.sh"
"${PROJECT_ROOT}/scripts/check_services.sh"
"${PROJECT_ROOT}/scripts/apply_migrations.sh"

"${PROJECT_ROOT}/scripts/run_postgres_integration_tests.sh"
"${PROJECT_ROOT}/scripts/run_redis_integration_tests.sh"
"${PROJECT_ROOT}/scripts/run_cached_integration_tests.sh"
