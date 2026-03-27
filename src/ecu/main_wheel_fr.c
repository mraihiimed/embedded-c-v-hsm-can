#include "ecu.h"
#include "config.h"
#include <unistd.h>

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "wheel_fr", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[WHEEL_FR] Front-right wheel ECU started\n");
    //log_msg(LOG_WARN, "[ATTACK] Sending unsecured frame!\n");
    uint8_t speed = 20;

    while (!ecu.fail_safe) {
        can_frame_t frame = {0};
        frame.id = 0x101;      // FR ID
        frame.dlc = 1;
        frame.data[0] = speed;
        frame.secured = true;
	//frame.secured = false;
        ecu_send(&ecu, &frame);

        speed = (speed + 2) % 200;
        usleep(100000);
    }

    return 0;
}
