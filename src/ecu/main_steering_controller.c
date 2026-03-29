#include "ecu.h"
#include "config.h"
#include <unistd.h>
#include "main_security_checks.h"

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "steering_controller", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[STEER_CTRL] Steering controller started\n");

    while (!ecu.fail_safe) {
        can_frame_t frame;
        if (!ecu_recv(&ecu, &frame)) break;

	// --- SECURITY CHECK (single call) ---
        security_status_t sec = security_check_frame(&ecu, &frame);
        // security_status_t sec = security_check_frame(&frame, "steering_controller");
        if (sec != SEC_OK) {
            ecu.fail_safe = true;
            security_failsafe("steering_controller");
            break;
        }
        // ------------------------------------
        if (frame.id == 0x300 && frame.dlc >= 1) {
            int8_t angle = (int8_t)frame.data[0];
            log_msg(LOG_INFO, "[STEER_CTRL] Steering angle command = %d°\n", angle);
        }
    }

    log_msg(LOG_ERROR, "[STEER_CTRL] FAIL-SAFE ACTIVE\n");
    return 0;
}

