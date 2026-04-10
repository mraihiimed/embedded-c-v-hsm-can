#include "mock_hsm_verify.h"

int mock_hsm_verify_return = 1;
int mock_hsm_verify_called = 0;

bool hsm_verify_mac(hsm_context_t *ctx, const can_frame_t *frame) {
    (void)ctx;
    (void)frame;
    mock_hsm_verify_called++;
    return mock_hsm_verify_return;
}
