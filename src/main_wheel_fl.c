#include "ecu.h"
#include "config.h"
#include <unistd.h>

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "wheel_fl", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    uint8_t speed = 10;
    while (!ecu.fail_safe) {
        can_frame_t frame = {0};
        frame.id = 0x100; // wheel FL ID
        frame.dlc = 1;
        frame.data[0] = speed;

        frame.secured = true;
        ecu_send(&ecu, &frame);

        speed = (speed + 1) % 200;
        usleep(100000); // 10 Hz
    }

    return 0;
}

