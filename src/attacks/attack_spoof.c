#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "bus.h"
#include "can_types.h"

void attack_spoof_send(uint32_t target_id, const uint8_t *payload, size_t len)
{
    can_frame_t frame;

    frame.id  = target_id;
    frame.dlc = (len > 8) ? 8 : (uint8_t)len;

    memset(frame.data, 0, sizeof(frame.data));
    if (payload != NULL && frame.dlc > 0) {
        memcpy(frame.data, payload, frame.dlc);
    }

    printf("[ATTACK_SPOOF] Sending spoofed frame to ID 0x%03X (dlc=%u)\n",
           frame.id, frame.dlc);

    bus_send_frame(&frame);
}
