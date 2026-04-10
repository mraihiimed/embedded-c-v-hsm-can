#ifndef MOCK_IDS_H
#define MOCK_IDS_H

#include "ids.h"

// Control variables
extern ids_state_t mock_ids_unsecured_return;
extern ids_state_t mock_ids_mac_error_return;

// Observability
extern int mock_ids_unsecured_called;
extern int mock_ids_mac_error_called;

// Mocked functions
ids_state_t ids_update_on_unsecured(ids_counters_t *c);
ids_state_t ids_update_on_mac_error(ids_counters_t *c);

#endif
