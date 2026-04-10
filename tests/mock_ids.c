#include "ids.h"

// Controls
ids_state_t mock_ids_unsecured_return = IDS_OK;
ids_state_t mock_ids_mac_error_return = IDS_OK;

// Observability
int mock_ids_unsecured_called = 0;
int mock_ids_mac_error_called = 0;

ids_state_t ids_update_on_unsecured(ids_counters_t *c) {
    (void)c;
    mock_ids_unsecured_called++;
    return mock_ids_unsecured_return;
}

ids_state_t ids_update_on_mac_error(ids_counters_t *c) {
    (void)c;
    mock_ids_mac_error_called++;
    return mock_ids_mac_error_return;
}
