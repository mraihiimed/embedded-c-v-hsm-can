#include "ecu.h"
#include "config.h"
#include <unistd.h>

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "brake_controller", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[BRAKE] Brake controller started\n");

    while (!ecu.fail_safe) {
        can_frame_t frame;
        if (!ecu_recv(&ecu, &frame)) break;

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

