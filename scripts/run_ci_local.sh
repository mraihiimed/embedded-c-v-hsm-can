#!/bin/bash
set -euo pipefail

echo "=== Local CI Simulation (Raspberry Pi) ==="

REPORTS="reports"
BUILD_DIR="build"
TEST_BUILD="$BUILD_DIR/tests"

echo "=== Preparing workspace ==="
mkdir -p "$REPORTS/coverage"
mkdir -p "$TEST_BUILD"

echo "=== Checking tools ==="
for tool in gcc make cppcheck python3; do
    command -v "$tool" >/dev/null || { echo "❌ $tool NOT FOUND"; exit 1; }
done
echo "✔ All required tools found"

echo
echo "=== Static Analysis: cppcheck ==="
cppcheck \
    --enable=all \
    --inconclusive \
    --force \
    --xml --xml-version=2 \
    -I include \
    --suppressions-list=tools/cppcheck/suppressions.txt \
    --platform=tools/cppcheck/platform.rules \
    src 2> "$REPORTS/cppcheck.xml"

echo "✔ cppcheck completed"

echo
echo "=== Building all ECUs (Makefile) ==="
make clean
make -j4
echo "✔ Build completed"

echo
echo "=== Building Unit Tests (Makefile test targets) ==="
make test
echo "✔ Unit test binaries built"

echo
echo "=== Running Unit Tests ==="
FOUND_TESTS=0

while IFS= read -r -d '' t; do
    FOUND_TESTS=1
    echo "==> Running $t"
    "$t" || { echo "❌ Test failed: $t"; exit 1; }
done < <(find "$TEST_BUILD" -maxdepth 1 -type f -executable -name "test_*" -print0)

if [[ "$FOUND_TESTS" -eq 0 ]]; then
    echo "⚠ No test binaries found in $TEST_BUILD"
else
    echo "✔ All unit tests passed"
fi

echo
echo "=== Converting Unity output to JUnit XML ==="
cat "$TEST_BUILD"/test_* > "$REPORTS/unity_output.txt"

python3 scripts/unity_to_junit.py \
    "$REPORTS/unity_output.txt" \
    "$REPORTS/junit.xml"

echo "✔ JUnit XML generated"

echo
echo "=== Coverage (Optional) ==="
if command -v lcov >/dev/null; then
    lcov --capture \
         --directory . \
         --output-file "$REPORTS/coverage.info" || echo "lcov failed"

    genhtml "$REPORTS/coverage.info" \
            --output-directory "$REPORTS/coverage" || echo "genhtml failed"

    echo "✔ Coverage generated"
else
    echo "⚠ lcov not installed — skipping coverage"
fi

echo
echo "=== Policy Gate ==="
ERRORS=$(grep -c "<error" "$REPORTS/cppcheck.xml" || true)

if [[ "$ERRORS" -gt 0 ]]; then
    echo "❌ Policy Gate failed: cppcheck found $ERRORS errors"
    exit 1
fi

echo "✔ Policy Gate passed"
echo "=== Local CI Completed Successfully ==="
