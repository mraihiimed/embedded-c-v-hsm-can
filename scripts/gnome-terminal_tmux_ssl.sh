#!/bin/bash

BASE_DIR="$HOME/Public/embedded-c-v-hsm-can/build/bin"
SESSION="cvhsm"

echo "[INFO] Starting C‑V‑HSM‑CAN multi‑pane dashboard..."
echo "[INFO] Using binaries from: $BASE_DIR"

# Kill old session if exists
tmux kill-session -t $SESSION 2>/dev/null

# Create session with first pane (bus server)
tmux new-session -d -s $SESSION -n dashboard "$BASE_DIR/bus_server"
sleep 0.3

# Create 11 more panes (total 12)
for i in {1..11}; do
    tmux split-window -t $SESSION:0
    tmux select-layout -t $SESSION:0 tiled
done

sleep 0.5

# Assign ECUs to panes
tmux send-keys -t $SESSION:0.0  "$BASE_DIR/bus_server" C-m
tmux send-keys -t $SESSION:0.1  "$BASE_DIR/monitor" C-m
tmux send-keys -t $SESSION:0.2  "$BASE_DIR/wheel_fl" C-m
tmux send-keys -t $SESSION:0.3  "$BASE_DIR/wheel_fr" C-m
tmux send-keys -t $SESSION:0.4  "$BASE_DIR/wheel_rl" C-m
tmux send-keys -t $SESSION:0.5  "$BASE_DIR/wheel_rr" C-m
tmux send-keys -t $SESSION:0.6  "$BASE_DIR/steering_sensor" C-m
tmux send-keys -t $SESSION:0.7  "$BASE_DIR/steering_controller" C-m
tmux send-keys -t $SESSION:0.8  "$BASE_DIR/brake_controller" C-m
tmux send-keys -t $SESSION:0.9  "$BASE_DIR/engine_ecu" C-m
tmux send-keys -t $SESSION:0.10 "$BASE_DIR/autonomous_controller" C-m

# Attacker (start last)
tmux send-keys -t $SESSION:0.11 "$BASE_DIR/attacker" C-m

# Final layout
tmux select-layout -t $SESSION:0 tiled

echo ""
echo "[INFO] Dashboard ready."
echo "[INFO] Attach with: tmux attach -t $SESSION"
echo ""
