#ifndef SECURITY_CHECKS_H
#define SECURITY_CHECKS_H

#include "can_types.h"
#include <stdbool.h>
#include "hsm.h"
typedef enum {
    SEC_OK = 0,
    SEC_ERR_MAC,
    SEC_ERR_DLC,
    SEC_ERR_SPOOF,
    SEC_ERR_REPLAY
} security_status_t;
security_status_t security_check_frame(hsm_context_t *hsm,
                                       const can_frame_t *frame,
                                       const char *ecu_name);

//security_status_t security_check_frame(const can_frame_t *frame, const char *ecu_name);
void security_failsafe(const char *ecu_name);

#endif
