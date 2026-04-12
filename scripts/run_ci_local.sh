#!/bin/bash
set -e

echo "=== Local CI Simulation (Raspberry Pi) ==="

REPORTS="reports"
BUILD_DIR="build"
TEST_BUILD="build/tests"

echo "=== Preparing workspace ==="
mkdir -p "$REPORTS"
mkdir -p "$REPORTS/coverage"
mkdir -p "$BUILD_DIR"
mkdir -p "$TEST_BUILD"

echo "=== Checking tools ==="
command -v gcc      >/dev/null || { echo "❌ gcc NOT FOUND"; exit 1; }
command -v make     >/dev/null || { echo "❌ make NOT FOUND"; exit 1; }
command -v cppcheck >/dev/null || { echo "❌ cppcheck NOT FOUND"; exit 1; }
command -v python3  >/dev/null || { echo "❌ python3 NOT FOUND"; exit 1; }

echo "✔ All required tools found"

echo "=== Static Analysis: cppcheck ==="
cppcheck --enable=all \
         --inconclusive \
         --force \
         --xml --xml-version=2 \
         -I include \
         --suppressions-list=tools/cppcheck/suppressions.txt \
         --platform=tools/cppcheck/platform.rules \
         src 2> "$REPORTS/cppcheck.xml"

echo "✔ cppcheck completed"

echo "=== Building all ECUs (Makefile) ==="
make clean
make -j4
echo "✔ Build completed"

echo "=== Building Unit Tests (Makefile test targets) ==="
make test
echo "✔ Unit test binaries built"

#echo "=== Running Unit Tests ==="
#for t in $TEST_BUILD/test_*; do
#    echo "==> Running $t"
#    "$t" || { echo "❌ Test failed: $t"; exit 1; }
#done
find "$TEST_BUILD" -maxdepth 1 -type f -executable -name "test_*" \
    -print0 | while IFS= read -r -d '' t; do
        echo "==> Running $t"
        "$t" || { echo "❌ Test failed: $t"; exit 1; }
    done

echo "✔ All unit tests passed"

echo "=== Converting Unity output to JUnit XML ==="
cat $TEST_BUILD/test_* > "$REPORTS/unity_output.txt"

python3 scripts/unity_to_junit.py \
    "$REPORTS/unity_output.txt" \
    "$REPORTS/junit.xml"

echo "✔ JUnit XML generated"

echo "=== Coverage (Optional) ==="
if command -v lcov >/dev/null; then
    lcov --capture --directory . --output-file "$REPORTS/coverage.info" || echo "lcov failed"
    genhtml "$REPORTS/coverage.info" --output-directory "$REPORTS/coverage" || echo "genhtml failed"
    echo "✔ Coverage generated"
else
    echo "⚠ lcov not installed — skipping coverage"
fi

echo "=== Policy Gate ==="
ERRORS=$(grep -c "<error" "$REPORTS/cppcheck.xml" || true)

if [ "$ERRORS" -gt 0 ]; then
    echo "❌ Policy Gate failed: cppcheck found $ERRORS errors"
    exit 1
fi

echo "✔ Policy Gate passed"
echo "=== Local CI Completed Successfully ==="
