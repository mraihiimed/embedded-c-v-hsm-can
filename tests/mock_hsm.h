#ifndef MOCK_HSM_H
#define MOCK_HSM_H

#include "hsm.h"
#include "can.h"
#include <stdbool.h>

// Control variables
extern int mock_hsm_init_return;
extern int mock_hsm_verify_return;

// Observability
extern int mock_hsm_init_called;
extern int mock_hsm_protect_called;
extern int mock_hsm_verify_called;

// Mocked functions
bool hsm_init(hsm_context_t *ctx, const char *ecu_name);
bool hsm_protect_frame(hsm_context_t *ctx, can_frame_t *frame);
bool hsm_verify_frame(hsm_context_t *ctx, const can_frame_t *frame);

#endif
