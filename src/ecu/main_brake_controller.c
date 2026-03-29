#include "ecu.h"
#include "config.h"
#include "can.h"
#include "network.h"
#include <unistd.h>
#include "main_security_checks.h"

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "brake_controller", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[BRAKE] Brake controller started\n");

    while (!ecu.fail_safe) {
        can_frame_t frame;
        if (!ecu_recv(&ecu, &frame)) break;

	// --- SECURITY CHECK (single call) ---
	security_status_t sec = security_check_frame(&ecu,&frame);

	// security_status_t sec = security_check_frame(&frame, "brake_controller");
        if (sec != SEC_OK) {
            ecu.fail_safe = true;
            security_failsafe("brake_controller");
            break;
        }
        // ------------------------------------


	//Normal brake command processing
        if (frame.id == 0x200 && frame.dlc >= 1) {
            uint8_t apply = frame.data[0];
            if (apply) {
                log_msg(LOG_INFO, "[BRAKE] Applying brake\n");
            } else {
                log_msg(LOG_INFO, "[BRAKE] Releasing brake\n");
            }
        }
    }

    log_msg(LOG_ERROR, "[BRAKE] FAIL-SAFE ACTIVE\n");
    return 0;
}

