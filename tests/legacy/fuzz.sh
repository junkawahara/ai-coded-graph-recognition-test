#!/usr/bin/env bash
# Fuzz testing wrapper for interval graph recognizers.
# This script is specific to interval graphs — it delegates to
# tests/compare.py which validates interval model certificates.
set -euo pipefail
bin1=${1:-./interval}
bin2=${2:?usage: fuzz.sh <bin1> <bin2> [cases] (two interval recognizer binaries)}
cases=${3:-200}
python3 "$(dirname "$0")/compare.py" "$bin1" "$bin2" "$cases"
