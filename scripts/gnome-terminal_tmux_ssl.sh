#!/bin/bash

BASE_DIR="$HOME/Public/embedded-c-v-hsm-can/build/bin"

echo "[INFO] Starting C‑V‑HSM‑CAN test environment..."

# Start tmux session with bus server
tmux new-session -d -s cvhsm "$BASE_DIR/bus_server"
sleep 0.3

# Monitor
tmux new-window -t cvhsm "$BASE_DIR/monitor"
sleep 0.3

# Wheel sensors
tmux new-window -t cvhsm "$BASE_DIR/wheel_fl"
tmux new-window -t cvhsm "$BASE_DIR/wheel_fr"
tmux new-window -t cvhsm "$BASE_DIR/wheel_rl"
tmux new-window -t cvhsm "$BASE_DIR/wheel_rr"
sleep 0.3

# Steering sensor
tmux new-window -t cvhsm "$BASE_DIR/steering_sensor"
sleep 0.2

# Engine ECU
tmux new-window -t cvhsm "$BASE_DIR/engine_ecu"
sleep 0.2

# Autonomous controller
tmux new-window -t cvhsm "$BASE_DIR/autonomous_controller"
sleep 0.2

echo ""
echo "[INFO] All ECUs started in tmux session 'cvhsm'."
echo "[INFO] Attach with: tmux attach -t cvhsm"
echo ""
