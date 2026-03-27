#!/bin/bash

echo "[INFO] Starting C‑V‑HSM‑CAN test environment..."

# Start bus server
gnome-terminal -- bash -c "./bus_server; exec bash"
sleep 0.3

# Start monitor
gnome-terminal -- bash -c "./monitor; exec bash"
sleep 0.3

# Start wheel sensors
gnome-terminal -- bash -c "./wheel_fl; exec bash"
gnome-terminal -- bash -c "./wheel_fr; exec bash"
gnome-terminal -- bash -c "./wheel_rl; exec bash"
gnome-terminal -- bash -c "./wheel_rr; exec bash"
sleep 0.3

# Start steering sensor
gnome-terminal -- bash -c "./steering_sensor; exec bash"
sleep 0.2

# Start engine ECU
gnome-terminal -- bash -c "./engine_ecu; exec bash"
sleep 0.2

# Start autonomous controller
gnome-terminal -- bash -c "./autonomous_controller; exec bash"
sleep 0.2

echo ""
echo "[INFO] All ECUs started in separate terminals."
echo "[INFO] Monitor window will now show clean output."
echo ""

