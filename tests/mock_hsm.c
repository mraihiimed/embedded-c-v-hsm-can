#include "hsm.h"
#include "can.h"

// Controls
int  mock_hsm_init_return   = 1;
int  mock_hsm_verify_return = 1;

// Observability
int mock_hsm_init_called    = 0;
int mock_hsm_protect_called = 0;
int mock_hsm_verify_called  = 0;

bool hsm_init(hsm_context_t *ctx, const char *ecu_name) {
    (void)ctx;
    (void)ecu_name;
    mock_hsm_init_called++;
    return mock_hsm_init_return;
}

bool hsm_protect_frame(hsm_context_t *ctx, can_frame_t *frame) {
    (void)ctx;
    mock_hsm_protect_called++;
    frame->secured = 1;
    return true;
}

bool hsm_verify_frame(hsm_context_t *ctx, const can_frame_t *frame) {
    (void)ctx;
    (void)frame;
    mock_hsm_verify_called++;
    return mock_hsm_verify_return;
}
