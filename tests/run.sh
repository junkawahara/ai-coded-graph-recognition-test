#!/usr/bin/env bash
set -euo pipefail
type=${1:?usage: run.sh <type> [binary]}
bin=${2:-./$type}
checker=tests/check_${type}.py

if [ ! -f "$checker" ]; then
  echo "No checker found: $checker"
  exit 1
fi

for in_file in tests/$type/*.in; do
  base=${in_file%.in}
  exp_file=${base}.exp
  out_file=${base}.got
  echo "[run] $in_file"
  "$bin" < "$in_file" > "$out_file"
  if ! python3 "$checker" "$in_file" "$exp_file" "$out_file"; then
    echo "FAIL: $in_file"
    exit 1
  fi
  rm -f "$out_file"
  echo "OK: $in_file"
  echo
done
