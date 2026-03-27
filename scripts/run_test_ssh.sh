#!/bin/bash
echo "[INFO] Starting C‑V‑HSM‑CAN test environment..."

# Start bus server
./bus_server > logs/bus_server.log 2>&1 &

# Start monitor
./monitor > logs/monitor.log 2>&1 &

# Wheel sensors
./wheel_fl > logs/wheel_fl.log 2>&1 &
./wheel_fr > logs/wheel_fr.log 2>&1 &
./wheel_rl > logs/wheel_rl.log 2>&1 &
./wheel_rr > logs/wheel_rr.log 2>&1 &

# Steering sensor
./steering_sensor > logs/steering_sensor.log 2>&1 &

# Engine ECU
./engine_ecu > logs/engine_ecu.log 2>&1 &

# Autonomous controller
./autonomous_controller > logs/autonomous_controller.log 2>&1 &

echo "[INFO] All ECUs started in background, logs in logs/ folder"
