---

# **Autonomous Vehicle CAN Bus System**  
### **Virtualized Automotive Network with HSM/IDS Security**  
### **Running on Raspberry Pi 4 (ARM Cortex‑A72, 64‑bit Linux)**

This project implements a realistic autonomous‑vehicle CAN bus architecture on a Raspberry Pi 4 using a process‑per‑ECU model, a virtual CAN bus over TCP, and a dedicated HSM/IDS security engine. It is designed for autonomous‑driving research, embedded‑systems education, and automotive cybersecurity experimentation.

The system includes:

- Sensor ECUs (wheel speeds, engine, steering)  
- Controller ECUs (autonomous driving logic)  
- Actuator ECUs (brake, throttle, steering)  
- A central CAN gateway (bus server)  
- A dedicated HSM service (MAC/CRC/Replay/AES‑GCM)  
- A real‑time monitor with decoding and security status  

All components run concurrently across the Raspberry Pi’s four CPU cores for deterministic performance.

---

## **1. Hardware Platform**

### Raspberry Pi 4 Model B

- 4× ARM Cortex‑A72 @ 1.5 GHz  
- ARMv8‑A (aarch64)  
- 4 GB LPDDR4  
- Linux 6.x (64‑bit)  
- Local TCP loopback for CAN bus  
- Unix domain sockets for HSM service  

---

## **2. Multi‑Core Execution Model**

Each ECU runs as its own Linux process pinned to a specific CPU core.

```
┌──────────────────────────────────────────────────────────────────┐
│  Raspberry Pi 4: 4× ARM Cortex‑A72 Cores                         │
├──────────┬──────────┬──────────┬────────────────────────────────┤
│  Core 0  │  Core 1  │  Core 2  │         Core 3                 │
│          │          │          │                                │
│  Bus     │ Sensors  │ Controls │  HSM Service (Crypto Engine)   │
│  Server  │  (TX)    │ (TX+RX)  │  ┌──────────────────────────┐  │
│  Monitor │ 6 ECUs:  │ 3 ECUs:  │  │ • MAC generation/verify  │  │
│          │ • Wheels │ • Auto   │  │ • CRC calculation/verify │  │
│          │   (4×)   │   Ctrl   │  │ • Replay protection      │  │
│          │ • Engine │ • Brake  │  │ • Anomaly detection      │  │
│          │ • Steer  │   Ctrl   │  │ • Session counters       │  │
│          │   Sensor │ • Steer  │  │ • Access control         │  │
│          │          │   Ctrl   │  │ • AES‑256‑GCM crypto     │  │
│          │          │          │  │ • Hardware RNG           │  │
│          │          │          │  └──────────────────────────┘  │
└──────────┴──────────┴──────────┴────────────────────────────────┘
```

---

## **3. System Architecture**

### Communication Layers

- **CAN Bus:** TCP server at `127.0.0.1:9000`  
- **HSM Service:** Unix domain socket `/tmp/vsm_hsm_service.sock`  
- **Monitor:** TCP client decoding all traffic  

### Message Flow

1. Sensors → CAN bus → Controller  
2. Controller → CAN bus → Actuators  
3. All ECUs → HSM for MAC/CRC  
4. All traffic → Monitor for decoding  

---

## **4. ECU Components**

### Infrastructure

- **bus_server** — Central TCP broadcast hub (virtual CAN gateway)  
- **monitor** — Real‑time dashboard with decoding and security status  

### Sensor ECUs

| ECU | Function | Rate |
|-----|----------|------|
| wheel_fl | Front‑Left wheel speed | 10 Hz |
| wheel_fr | Front‑Right wheel speed | 10 Hz |
| wheel_rl | Rear‑Left wheel speed | 10 Hz |
| wheel_rr | Rear‑Right wheel speed | 10 Hz |
| engine_ecu | RPM + throttle | 20 Hz |
| steering_sensor | Steering angle + torque | 20 Hz |

### Controller ECU

- **autonomous_controller**  
  - Reads all sensor data  
  - Computes brake, throttle, steering  
  - Performs anomaly detection  
  - Sends commands at 10 Hz  

### Actuator ECUs

- **brake_controller** — Applies brake pressure  
- **steering_controller** — Applies steering angle  

---

## **5. CAN Message IDs**

### Sensor Frames (0x100–0x1FF)

| ID | Message | DLC | Rate |
|----|---------|-----|------|
| 0x100 | Wheel Speed FL | 2 | 10 Hz |
| 0x101 | Wheel Speed FR | 2 | 10 Hz |
| 0x102 | Wheel Speed RL | 2 | 10 Hz |
| 0x103 | Wheel Speed RR | 2 | 10 Hz |
| 0x110 | Engine RPM | 2 | 20 Hz |
| 0x111 | Throttle Position | 1 | 20 Hz |
| 0x120 | Steering Angle | 2 | 20 Hz |
| 0x121 | Steering Torque | 2 | 20 Hz |

### Control Frames (0x300–0x3FF)

| ID | Message | DLC | Rate |
|----|---------|-----|------|
| 0x300 | Brake Command | 1 | 10 Hz |
| 0x301 | Throttle Command | 1 | 10 Hz |
| 0x302 | Steering Command | 2 | 10 Hz |

---

## **6. Core 3 HSM Service (Crypto Engine)**

The HSM is the security brain of the system. It performs:

- **MAC generation/verification** (HMAC‑SHA256)  
- **CRC32 integrity checks**  
- **Replay protection** (timestamps + session counters)  
- **Anomaly detection** (DoS, spoofing, unauthorized IDs)  
- **Access control** (TX whitelist per ECU)  
- **AES‑256‑GCM encryption/decryption**  
- **Hardware RNG** for nonces, keys, IVs  

### HSM Responsibilities

- Authenticate every CAN frame  
- Enforce ECU → CAN ID authorization  
- Detect replayed or stale messages  
- Detect high‑rate DoS patterns  
- Provide secure boot and firmware verification  
- Maintain tamper‑evident security logs  

---

## **7. Running the System**

### Single‑Command Launch

```
cd autonomous_controller
cargo run
```

This automatically:

- Starts HSM on Core 3  
- Starts bus_server on Core 0  
- Launches sensors on Core 1  
- Launches controllers on Core 2  
- Opens dashboard on Core 0  

Press **q** to quit.

---

## **8. Manual Multi‑Terminal Launch**

### Terminal 1 — Bus Server

```
cargo run --bin bus_server
```

### Terminal 2 — Monitor

```
cargo run --bin monitor
```

### Terminals 3–8 — Sensors

```
cargo run --bin wheel_fl
cargo run --bin wheel_fr
cargo run --bin wheel_rl
cargo run --bin wheel_rr
cargo run --bin engine_ecu
cargo run --bin steering_sensor
```

### Terminal 9 — Autonomous Controller

```
cargo run --bin autonomous_controller
```

### Terminals 10–11 — Actuators

```
cargo run --bin brake_controller
cargo run --bin steering_controller
```

---

## **9. Dashboard Features**

- Real‑time sensor values  
- Controller commands  
- Actuator states  
- Security status (MAC/CRC/Replay)  
- Attack banners  
- Timestamped logs  

---

## **10. Data Encoding**

- Wheel Speed: u16, 0.01 rad/s  
- Engine RPM: u16, 0.25 RPM  
- Throttle/Brake: u8, 1%  
- Steering Angle: u16, 0.1°  
- Steering Torque: u16, 0.001 Nm  

---

## **11. Security Testing Scenarios**

- Spoofing (invalid MAC/CRC)  
- Replay (old timestamp)  
- DoS (high‑rate flooding)  
- Unauthorized ID injection  
- Controller fail‑safe activation  

---

## **12. Attack Simulations**

- Spoof ECU  
- Replay injector  
- DoS flooder  
- Mixed multi‑vector attacks  

All attacks are safely contained within the virtual environment.

---

## **13. Performance**

- >10 000 CAN messages/sec  
- HSM latency: 35–70 µs  
- CPU affinity ensures deterministic timing  
- Async Rust runtime (Tokio)  
- Zero‑copy frame forwarding  

---

## **14. Project Structure**

```
autonomous_controller/
├── src/
│   ├── types.rs
│   ├── can_bus.rs
│   ├── ecu.rs
│   ├── network.rs
│   ├── lib.rs
│   └── bin/
│       ├── bus_server.rs
│       ├── monitor.rs
│       ├── wheel_fl.rs
│       ├── wheel_fr.rs
│       ├── wheel_rl.rs
│       ├── wheel_rr.rs
│       ├── engine_ecu.rs
│       ├── steering_sensor.rs
│       ├── autonomous_controller.rs
│       ├── brake_controller.rs
│       └── steering_controller.rs
├── wireshark/
│   └── vhsm_can.lua
├── Cargo.toml
└── README.md
```

---

## **15. Wireshark Integration**

```
sudo tshark -X lua_script:wireshark/vhsm_can.lua -i lo -f "tcp port 9000"
```

Decoded fields include:

- CAN ID  
- ECU name  
- Data (raw + physical units)  
- MAC  
- CRC  
- Timestamp  
- Session counter  

---

## **16. Future Enhancements**

- CAN‑FD support  
- ISO‑TP multi‑frame  
- GUI monitor  
- ML‑based anomaly detection  
- Hardware CAN transceiver support  
- Real‑time scheduling (RT‑PREEMPT)  

---
