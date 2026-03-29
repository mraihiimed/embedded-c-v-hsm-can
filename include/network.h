#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "can_types.h"
#include "can.h"

int net_connect_to_bus(const char *host, uint16_t port);
int net_listen_bus(uint16_t port);
bool net_send_frame(int sockfd, const can_frame_t *frame);
bool net_recv_frame(int sockfd, can_frame_t *frame);
int network_init(void);
int network_send_frame(const can_frame_t *frame);

#endif

