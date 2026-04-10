#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "network.h"

// Control
extern int  mock_connect_return;
extern bool mock_send_return;
extern bool mock_recv_return;

// Captured
extern int         mock_last_sockfd;
extern can_frame_t mock_last_sent_frame;
extern const char *mock_last_host;
extern uint16_t    mock_last_port;

// Reset
void mock_net_lowlevel_reset(void);
