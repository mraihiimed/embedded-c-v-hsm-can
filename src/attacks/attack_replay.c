#include <stdio.h>

#include "bus.h"
#include "can_types.h"

void attack_replay_frame(const can_frame_t *captured)
{
    if (captured == NULL) {
        printf("[ATTACK_REPLAY] No frame to replay (NULL pointer)\n");
        return;
    }

    printf("[ATTACK_REPLAY] Replaying frame ID 0x%03X (dlc=%u)\n",
           captured->id, captured->dlc);

    bus_send_frame(captured);
}
