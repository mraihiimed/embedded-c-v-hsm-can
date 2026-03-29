#!/bin/bash


#killall bus_server monitor wheel_fl wheel_fr wheel_rl wheel_rr steering_sensor engine_ecu autonomous_controller 2>/dev/null


SESSION="cvhsm"

echo "[INFO] Killing tmux session '$SESSION'..."

tmux kill-session -t $SESSION 2>/dev/null

echo "[INFO] All ECUs stopped."
