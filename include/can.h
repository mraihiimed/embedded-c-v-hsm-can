#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stdbool.h>

#define CAN_MAX_DATA_LEN 8
#define CAN_MAC_LEN      32

// Standard CAN IDs
#define CAN_ID_WHEEL_FL       0x100
#define CAN_ID_WHEEL_FR       0x101
#define CAN_ID_WHEEL_RL       0x102
#define CAN_ID_WHEEL_RR       0x103
#define CAN_ID_ENGINE         0x110
#define CAN_ID_STEER          0x120

#define CAN_ID_BRAKE_CMD      0x300
#define CAN_ID_THROTTLE_CMD   0x301
#define CAN_ID_STEER_CMD      0x302
#define CAN_ID_SECURITY_EVENT 0x3F0


// Unified CAN frame structure
typedef struct {
    uint32_t id;                     // CAN ID
    uint8_t  dlc;                    // Data length (0–8)
    uint8_t  data[CAN_MAX_DATA_LEN]; // Payload bytes

    uint32_t crc;                    // CRC32
    uint8_t  mac[CAN_MAC_LEN];       // HMAC-SHA256 (32 bytes)

    bool     secured;                // true = MAC+CRC valid
    uint64_t timestamp;              // microseconds or milliseconds
} can_frame_t;

#endif
