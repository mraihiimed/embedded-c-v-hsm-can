#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "bus.h"
#include "can_types.h"

static int fuzz_rng_initialized = 0;

static void fuzz_init_rng(void)
{
    if (!fuzz_rng_initialized) {
        srand((unsigned int)time(NULL));
        fuzz_rng_initialized = 1;
    }
}

void attack_fuzz_random(uint32_t id)
{
    fuzz_init_rng();

    can_frame_t frame;
    frame.id  = id;
    frame.dlc = (uint8_t)(rand() % 9); // 0..8

    for (int i = 0; i < frame.dlc; i++) {
        frame.data[i] = (uint8_t)(rand() & 0xFF);
    }
    for (int i = frame.dlc; i < 8; i++) {
        frame.data[i] = 0;
    }

    printf("[ATTACK_FUZZ] Fuzzing ID 0x%03X (dlc=%u)\n", frame.id, frame.dlc);

    bus_send_frame(&frame);
}
