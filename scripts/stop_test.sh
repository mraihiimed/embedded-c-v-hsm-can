#!/bin/bash

echo "[INFO] Stopping all ECUs..."

killall bus_server monitor wheel_fl wheel_fr wheel_rl wheel_rr steering_sensor engine_ecu autonomous_controller 2>/dev/null

echo "[INFO] Done."
