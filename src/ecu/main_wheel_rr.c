#include "ecu.h"
#include "config.h"
#include <unistd.h>

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "wheel_rr", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[WHEEL_RR] Rear-right wheel ECU started\n");

    uint8_t speed = 18;

    while (!ecu.fail_safe) {
        can_frame_t frame = {0};
        frame.id = CAN_ID_WHEEL_RR;
        frame.dlc = 1;
        frame.data[0] = speed;
        frame.secured = true;

        ecu_send(&ecu, &frame);

        speed = (speed + 4) % 200;
        usleep(100000);
    }
    log_msg(LOG_ERROR, "[WHEEL_FL], FAIL-SAFE ACTIVE\n");
    return 0;
}

