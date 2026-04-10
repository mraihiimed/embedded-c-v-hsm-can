#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "can.h"   // <-- ONLY SOURCE OF can_frame_t

// ---------------------------------------------------------------------------
// High-level API
// ---------------------------------------------------------------------------

int network_init(void);
int network_send_frame(const can_frame_t *frame);

// ---------------------------------------------------------------------------
// Low-level API (mocked or real implementation)
// ---------------------------------------------------------------------------

int net_connect_to_bus(const char *host, uint16_t port);
bool net_send_frame(int sockfd, const can_frame_t *frame);
bool net_recv_frame(int sockfd, can_frame_t *frame);
int net_listen_bus(uint16_t port);
// ---------------------------------------------------------------------------
// Test support
// ---------------------------------------------------------------------------
#ifdef UNIT_TEST
void network_reset(void);
#endif

#endif
