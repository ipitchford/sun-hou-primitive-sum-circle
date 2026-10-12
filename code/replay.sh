#!/bin/sh
# Verify frozen certificates without modifying any source or certificate.
# Optional --regenerate searches in a temporary directory and compares bytes.
set -eu
cd "$(dirname "$0")/.."
python3 code/build_supplement.py --check
exec python3 code/frozen_check.py "$@"
