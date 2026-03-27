#!/bin/bash

set -e

echo "Reorganizing project structure..."

# ---------------------------------------------------------------------------
# Create new directory structure
# ---------------------------------------------------------------------------
mkdir -p src/core
mkdir -p src/ecu
mkdir -p src/build
mkdir -p build/bin
mkdir -p build/logs
mkdir -p scripts

# ---------------------------------------------------------------------------
# Move core source files
# ---------------------------------------------------------------------------
CORE_FILES=(
    bus.c ecu.c hsm.c ids.c network.c util.c protected_memory.c
)

for f in "${CORE_FILES[@]}"; do
    if [ -f "src/$f" ]; then
        mv "src/$f" src/core/
    fi
done

# ---------------------------------------------------------------------------
# Move ECU main files
# ---------------------------------------------------------------------------
for f in src/main_*.c; do
    [ -e "$f" ] || continue
    mv "$f" src/ecu/
done

# ---------------------------------------------------------------------------
# Move object and dependency files
# ---------------------------------------------------------------------------
for f in src/*.o src/*.d; do
    [ -e "$f" ] || continue
    mv "$f" src/build/
done

# ---------------------------------------------------------------------------
# Move binaries to build/bin
# ---------------------------------------------------------------------------
ECU_BINS=(
    bus_server monitor wheel_fl wheel_fr wheel_rl wheel_rr
    steering_sensor steering_controller brake_controller
    autonomous_controller engine_ecu
)

for bin in "${ECU_BINS[@]}"; do
    if [ -f "$bin" ]; then
        mv "$bin" build/bin/
    fi
done

# Also move .bin copies if they exist
if [ -d "bin" ]; then
    mv bin/*.bin build/bin/ 2>/dev/null || true
fi

# ---------------------------------------------------------------------------
# Move logs
# ---------------------------------------------------------------------------
if [ -d "logs" ]; then
    mv logs/*.log build/logs/ 2>/dev/null || true
fi

# ---------------------------------------------------------------------------
# Move scripts
# ---------------------------------------------------------------------------
for s in run_test.sh run_test_ssh.sh stop_test.sh; do
    if [ -f "$s" ]; then
        mv "$s" scripts/
    fi
done

echo "Reorganization complete."
echo "New structure is ready."
