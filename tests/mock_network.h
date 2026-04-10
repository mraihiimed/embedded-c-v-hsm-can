#ifndef MOCK_NETWORK_H
#define MOCK_NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "can.h"

// --- Control values set by tests ---
extern int  mock_connect_return;
extern int  mock_listen_return;
extern bool mock_send_return;
extern bool mock_recv_return;

// --- Legacy counter required by test_bus ---
extern int mock_network_send_called;

// --- Captured values for assertions ---
extern int mock_last_sockfd;
extern can_frame_t mock_last_frame;

// --- Mocked low-level functions ---
int  net_connect_to_bus(const char *host, uint16_t port);
int  net_listen_bus(uint16_t port);
bool net_send_frame(int sockfd, const can_frame_t *frame);
bool net_recv_frame(int sockfd, can_frame_t *frame);

// --- Mocked high-level API (required by bus_testable, ecu, ids tests) ---
int network_send_frame(const can_frame_t *frame);
int network_receive_frame(can_frame_t *frame);

#endif
