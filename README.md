# Autonomous Vehicle CAN Bus System (C Implementation)

### Virtualized Automotive Network with HSM / IDS Security

### Raspberry Pi 4 • ARM Cortex-A72 • Linux (64-bit)

---

## Overview

This project implements a **realistic automotive CAN bus architecture** using **pure C**, designed for:

* Embedded systems education
* Automotive cybersecurity research
* Autonomous driving simulations

The system runs on a **Raspberry Pi 4** and simulates a complete vehicle network with:

* Sensor ECUs
* Control ECUs
* Actuator ECUs
* Virtual CAN bus (TCP-based)
* Hardware Security Module (HSM)
* Real-time monitoring dashboard

All components are implemented as **independent Linux processes**, enabling **deterministic multi-core execution**.

---

## Key Features

* Pure **C99 + POSIX APIs**
* Multi-process architecture (real ECU simulation)
* CPU affinity (`sched_setaffinity`) for deterministic timing
* Virtual CAN bus over TCP (`127.0.0.1:9000`)
* Unix domain socket-based HSM service
* Integrated IDS (Intrusion Detection System)
* Real-time monitoring and decoding
* Attack simulation environment

---

## Hardware Platform

* Raspberry Pi 4 Model B
* 4× ARM Cortex-A72 @ 1.5 GHz
* 64-bit Linux (Kernel 6.x)
* 4 GB RAM

---

## System Architecture

### Communication Layers

| Component | Technology                     |
| --------- | ------------------------------ |
| CAN Bus   | TCP Socket Server              |
| HSM       | Unix Domain Socket (`AF_UNIX`) |
| Monitor   | TCP Client                     |

### Message Flow

```
Sensors → Bus Server → Controller → Bus Server → Actuators
        ↘ HSM (MAC / CRC / Replay Protection)
        ↘ Monitor (Decoding + Logging)
```

---

## Multi-Core Execution Model

Each ECU runs as a **separate process pinned to a CPU core**:

| Core   | Components           |
| ------ | -------------------- |
| Core 0 | Bus Server + Monitor |
| Core 1 | Sensor ECUs          |
| Core 2 | Controller ECUs      |
| Core 3 | HSM Service          |

---

## ECU Components

### Infrastructure

* `bus_server` — Virtual CAN gateway (TCP broadcast)
* `monitor` — Real-time system dashboard

---

### Sensor ECUs

| ECU             | Function          | Rate  |
| --------------- | ----------------- | ----- |
| wheel_fl        | Front-left speed  | 10 Hz |
| wheel_fr        | Front-right speed | 10 Hz |
| wheel_rl        | Rear-left speed   | 10 Hz |
| wheel_rr        | Rear-right speed  | 10 Hz |
| engine_ecu      | RPM + throttle    | 20 Hz |
| steering_sensor | Angle + torque    | 20 Hz |

---

### Controller

* `autonomous_controller`

  * Aggregates sensor data
  * Computes vehicle commands
  * Performs anomaly detection

---

### Actuators

* `brake_controller`
* `steering_controller`

---

## CAN Message IDs

### Sensor Frames (0x100–0x1FF)

| ID          | Signal          | Size    |
| ----------- | --------------- | ------- |
| 0x100–0x103 | Wheel speeds    | 2 bytes |
| 0x110       | Engine RPM      | 2 bytes |
| 0x111       | Throttle        | 1 byte  |
| 0x120       | Steering angle  | 2 bytes |
| 0x121       | Steering torque | 2 bytes |

---

### Control Frames (0x300–0x3FF)

| ID    | Signal           |
| ----- | ---------------- |
| 0x300 | Brake command    |
| 0x301 | Throttle command |
| 0x302 | Steering command |

---

## HSM Service (Security Core)

Standalone process providing:

* HMAC-SHA256 authentication
* CRC32 integrity verification
* Replay protection (timestamps + counters)
* DoS detection
* ECU authorization (TX whitelist)
* AES-256-GCM encryption
* Secure random generation (`/dev/urandom`)

### Security Responsibilities

* Authenticate every frame
* Detect spoofing and replay attacks
* Enforce ECU permissions
* Monitor traffic anomalies

---

## Build

```bash
make
```

---

## Run (Recommended)

```bash
./run_all.sh
```

This will:

* Launch all ECUs
* Assign CPU cores
* Start HSM, bus server, and monitor

Press `q` to quit.

---

## Manual Execution

### Infrastructure

```bash
./bin/bus_server
./bin/monitor
```

---

### Sensors

```bash
./bin/wheel_fl
./bin/wheel_fr
./bin/wheel_rl
./bin/wheel_rr
./bin/engine_ecu
./bin/steering_sensor
```

---

### Controller

```bash
./bin/autonomous_controller
```

---

### Actuators

```bash
./bin/brake_controller
./bin/steering_controller
```

---

## Data Encoding

| Signal         | Type     | Scale      |
| -------------- | -------- | ---------- |
| Wheel speed    | uint16_t | 0.01 rad/s |
| RPM            | uint16_t | 0.25 RPM   |
| Throttle/Brake | uint8_t  | 1%         |
| Steering angle | uint16_t | 0.1°       |
| Torque         | uint16_t | 0.001 Nm   |

---

## Dashboard Features

* Real-time ECU data
* Command visualization
* Security status (MAC / CRC / Replay)
* Attack detection alerts
* Timestamped logs

---

## Security Testing

Supported attack scenarios:

* Spoofed frames (invalid MAC)
* Replay attacks
* DoS flooding
* Unauthorized CAN IDs
* Controller fail-safe triggers

---

## Attack Simulation

Includes dedicated attacker modules:

* Spoofing ECU
* Replay injector
* DoS generator
* Multi-vector attacks

All attacks are contained within the virtual environment.

---

## Performance

* > 10,000 messages/sec
* HSM latency: ~35–70 µs
* Deterministic scheduling via CPU pinning
* Zero-copy frame forwarding
* Minimal memory footprint

---

## Project Structure

```
project_root/
├── src/
│   ├── can_drv.c
│   ├── hsm_client.c
│   ├── utils.c
│   └── ecus/
│       ├── bus_server.c
│       ├── monitor.c
│       ├── sensors/
│       ├── controller/
│       └── actuators/
├── wireshark/
│   └── vhsm_can.lua
├── Makefile
└── README.md
```

---

## Wireshark Integration

```bash
sudo tshark -X lua_script:wireshark/vhsm_can.lua -i lo -f "tcp port 9000"
```

Decodes:

* CAN ID
* ECU name
* Physical values
* MAC / CRC
* Timestamp
* Session counters

---

## Future Enhancements

* CAN-FD support
* ISO-TP multi-frame transport
* GUI-based monitor
* Machine-learning IDS
* Real CAN hardware integration
* Real-time Linux (RT-PREEMPT)

---

## License

This project is intended for:

* Education
* Research
* Security experimentation

---
