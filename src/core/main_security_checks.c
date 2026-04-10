#include "main_security_checks.h"
#include "can.h"
#include "hsm.h"
#include <stdio.h>

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

security_status_t security_check_frame(hsm_context_t *hsm,
                                       const can_frame_t *frame,
                                       const char *ecu_name)
{
    if (!frame) return SEC_ERR_DLC;

    // 1. DLC sanity check
    if (frame->dlc > CAN_MAX_DATA_LEN) {
        printf("[ERROR] [%s] Malformed frame (DLC=%d)\n", ecu_name, frame->dlc);
        return SEC_ERR_DLC;
    }

    // 2. ID whitelist
    if (!id_allowed(frame->id)) {
        printf("[IDS] [%s] Spoofed ID 0x%03X detected\n", ecu_name, frame->id);
        return SEC_ERR_SPOOF;
    }

    if (!hsm_verify_mac(hsm, frame)) {
    printf("[WARN] [HSM] MAC mismatch on ECU '%s'\n", ecu_name);
    return SEC_ERR_MAC;
    }


    return SEC_OK;
}

void security_failsafe(const char *ecu_name)
{
    printf("[FAILSAFE] Activated on ECU '%s'\n", ecu_name);

    // You can customize per ECU later
    // For now, this is a global fail-safe trigger
}
