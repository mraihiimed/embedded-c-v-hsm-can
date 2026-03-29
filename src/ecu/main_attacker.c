#include <stdio.h>
#include <stdint.h>

#include "network.h"
#include "bus.h"
#include "util.h"
#include "can.h"   // your CAN ID definitions
#include "attacks/attack_spoof.h"
#include "attacks/attack_fuzz.h"
#include "attacks/attack_malformed.h"
#include "attacks/attack_menu.h"

/*int main(void)
{
    if (network_init() < 0) {
        printf("[ATTACKER] Network init failed, exiting\n");
        return 1;
    }

    bus_init();

    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    if (sock < 0) {
        printf("[ATTACKER] Cannot connect to bus\n");
        return 1;
    }

    run_attack_menu(sock);

    return 0;
}*/
#include "attacks/attack_menu.h"

int main(void)
{
    network_init();
    bus_init();

    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    run_attack_menu(sock);
}
