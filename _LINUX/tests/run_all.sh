#!/bin/bash
# Run every test_NN_*.sh in numeric order. Each test is self-contained so
# failures don't cascade — set +e lets the next one run even if one fails.
#
# Usage from repo root:
#   ./_LINUX/tests/run_all.sh
#
# Skip the slow YOLO test by exporting SKIP_YOLO=1.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Iterate over the ordered list of tests.
for t in "$SCRIPT_DIR"/test_0*.sh; do
    if [[ "${SKIP_YOLO:-0}" == "1" && "$t" == *"test_07_"* ]]; then
        echo "SKIP $t"; continue
    fi
    echo
    echo "########################################"
    echo "# RUNNING $(basename "$t")"
    echo "########################################"
    bash "$t"
done