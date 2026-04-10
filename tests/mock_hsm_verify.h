#ifndef MOCK_HSM_VERIFY_H
#define MOCK_HSM_VERIFY_H

#include "hsm.h"
#include "can.h"

// Control variable
extern int mock_hsm_verify_return;

// Observability
extern int mock_hsm_verify_called;

// Mocked function (matches main_security_checks.c)
bool hsm_verify_mac(hsm_context_t *ctx, const can_frame_t *frame);

#endif
