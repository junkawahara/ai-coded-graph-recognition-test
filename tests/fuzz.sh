#!/usr/bin/env bash
set -euo pipefail
bin1=${1:-./interval}
bin2=${2:-./interval_at}
cases=${3:-200}
python3 ./tests/compare.py "$bin1" "$bin2" "$cases"
