#ifndef SECURITY_CHECKS_H
#define SECURITY_CHECKS_H

#include "can_types.h"
#include <stdbool.h>
#include "hsm.h"
#include "ecu.h"
// Security event CAN ID
#define CAN_ID_SECURITY_EVENT 0x3F0

// Security event types
#define SEC_EVT_MAC_FAIL   1   // MAC mismatch
#define SEC_EVT_SPOOF      2   // Spoofed CAN ID
#define SEC_EVT_DLC_ERR    3   // Malformed DLC
#define SEC_EVT_REPLAY     4   // Replay attack
#define SEC_EVT_DOS        5   // DoS / flooding


typedef enum {
    SEC_OK = 0,
    SEC_ERR_MAC,
    SEC_ERR_DLC,
    SEC_ERR_SPOOF,
    SEC_ERR_REPLAY
} security_status_t;

security_status_t security_check_frame(ecu_t *ecu,
                                       const can_frame_t *frame);
void security_failsafe(const char *ecu_name);
#endif
