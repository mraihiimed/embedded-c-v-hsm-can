#include "main_security_checks.h"
#include "ecu.h"
#include "can.h"
#include "hsm.h"
#include <stdio.h>

// -------------------------------
// Allowed CAN IDs
// -------------------------------
static bool id_allowed(uint32_t id)
{
    switch (id) {
        case CAN_ID_WHEEL_FL:
        case CAN_ID_WHEEL_FR:
        case CAN_ID_WHEEL_RL:
        case CAN_ID_WHEEL_RR:
        case CAN_ID_ENGINE:
        case CAN_ID_STEER:
        case CAN_ID_BRAKE_CMD:
        case CAN_ID_THROTTLE_CMD:
        case CAN_ID_STEER_CMD:
            return true;
        default:
            return false;
    }
}

// -------------------------------
// Send security event to monitor
// -------------------------------
static void send_security_event(ecu_t *ecu, uint8_t type, const can_frame_t *bad)
{
    can_frame_t evt = {0};
    evt.id  = CAN_ID_SECURITY_EVENT;
    evt.dlc = 3;
    evt.data[0] = type;          // event type
    evt.data[1] = ecu->ecu_id;   // which ECU detected it
    evt.data[2] = bad->id & 0xFF;

    ecu_send(ecu, &evt);
}

// -------------------------------
// Main security check
// -------------------------------
security_status_t security_check_frame(ecu_t *ecu,
                                       const can_frame_t *frame)
{
    if (!frame) return SEC_ERR_DLC;

    // 1. DLC sanity check
    if (frame->dlc > CAN_MAX_DATA_LEN) {
        send_security_event(ecu, SEC_EVT_DLC_ERR, frame);
        printf("[ERROR] [%s] Malformed frame (DLC=%d)\n",
               ecu->name, frame->dlc);
        return SEC_ERR_DLC;
    }

    // 2. ID whitelist
    if (!id_allowed(frame->id)) {
        send_security_event(ecu, SEC_EVT_SPOOF, frame);
        printf("[IDS] [%s] Spoofed ID 0x%03X detected\n",
               ecu->name, frame->id);
        return SEC_ERR_SPOOF;
    }

    // 3. MAC verification
    if (!hsm_verify_mac(&ecu->hsm, frame)) {
        send_security_event(ecu, SEC_EVT_MAC_FAIL, frame);
        printf("[WARN] [HSM] MAC mismatch on ECU '%s'\n", ecu->name);
        return SEC_ERR_MAC;
    }

    return SEC_OK;
}

// -------------------------------
// Fail-safe handler
// -------------------------------
void security_failsafe(const char *ecu_name)
{
    printf("[FAILSAFE] Activated on ECU '%s'\n", ecu_name);
}
