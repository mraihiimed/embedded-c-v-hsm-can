#include "unity.h"
#include "ecu.h"
#include "hsm.h"
#include "ids.h"
#include "network.h"
#include "util.h"
#include "mock_hsm_verify.h"
#include "mock_network.h"
#include "mock_hsm.h"
#include "mock_ids.h"
#include "mock_log.h"

void setUp(void) {
    mock_connect_return = 1;
    mock_send_return = 1;
    mock_recv_return = 0;

    mock_hsm_init_return = 1;
    mock_hsm_verify_return = 1;

    mock_ids_unsecured_return = IDS_OK;
    mock_ids_mac_error_return = IDS_OK;

    mock_network_send_called = 0;
}

void tearDown(void) {}

/* -------------------------------------------------------------
 * TESTS
 * ------------------------------------------------------------- */

void test_ecu_init_success(void) {
    ecu_t ecu;
    TEST_ASSERT_TRUE(ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000));
}

void test_ecu_init_fails_when_network_fails(void) {
    mock_connect_return = -1;

    ecu_t ecu;
    TEST_ASSERT_FALSE(ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000));
}

void test_ecu_init_fails_when_hsm_fails(void) {
    mock_hsm_init_return = 0;

    ecu_t ecu;
    TEST_ASSERT_FALSE(ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000));
}

void test_ecu_send_unsecured_triggers_fail_safe(void) {
    ecu_t ecu;
    ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000);

    can_frame_t f = { .id = 0x100, .dlc = 1, .secured = 0 };

    mock_ids_unsecured_return = IDS_FAIL_SAFE;

    TEST_ASSERT_FALSE(ecu_send(&ecu, &f));
    TEST_ASSERT_TRUE(ecu.fail_safe);
}

void test_ecu_send_secured_calls_hsm_and_network(void) {
    ecu_t ecu;
    ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000);

    can_frame_t f = { .id = 0x100, .dlc = 1, .secured = 1 };

    TEST_ASSERT_TRUE(ecu_send(&ecu, &f));

    TEST_ASSERT_EQUAL(1, mock_network_send_called);
    TEST_ASSERT_EQUAL(1, mock_last_frame.secured);
}

void test_ecu_recv_unsecured_updates_ids(void) {
    ecu_t ecu;
    ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000);

    mock_recv_return = 1;
    mock_last_frame.secured = 0;

    mock_ids_unsecured_return = IDS_FAIL_SAFE;

    can_frame_t f;
    TEST_ASSERT_TRUE(ecu_recv(&ecu, &f));
    TEST_ASSERT_TRUE(ecu.fail_safe);
}

void test_ecu_recv_secured_mac_error(void) {
    ecu_t ecu;
    ecu_init(&ecu, "ECU_A", "127.0.0.1", 5000);

    mock_recv_return = 1;
    mock_last_frame.secured = 1;

    mock_hsm_verify_return = 0; // MAC fail
    mock_ids_mac_error_return = IDS_FAIL_SAFE;

    can_frame_t f;
    TEST_ASSERT_TRUE(ecu_recv(&ecu, &f));
    TEST_ASSERT_TRUE(ecu.fail_safe);
}

/* -------------------------------------------------------------
 * UNITY MAIN
 * ------------------------------------------------------------- */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ecu_init_success);
    RUN_TEST(test_ecu_init_fails_when_network_fails);
    RUN_TEST(test_ecu_init_fails_when_hsm_fails);
    RUN_TEST(test_ecu_send_unsecured_triggers_fail_safe);
    RUN_TEST(test_ecu_send_secured_calls_hsm_and_network);
    RUN_TEST(test_ecu_recv_unsecured_updates_ids);
    RUN_TEST(test_ecu_recv_secured_mac_error);

    return UNITY_END();
}
