#include "unity.h"
#include "main_security_checks.h"
#include "can_types.h"
#include "mock_hsm_verify.h"

void setUp(void) {
    mock_hsm_verify_return = 1;
}

void tearDown(void) {}

void test_security_check_null_frame_should_return_dlc_error(void) {
    hsm_context_t ctx;
    TEST_ASSERT_EQUAL(SEC_ERR_DLC, security_check_frame(&ctx, NULL, "ECU1"));
}

void test_security_check_invalid_dlc_should_fail(void) {
    hsm_context_t ctx;
    can_frame_t frame = {
        .id = CAN_ID_ENGINE,
        .dlc = 20 // invalid
    };

    TEST_ASSERT_EQUAL(SEC_ERR_DLC, security_check_frame(&ctx, &frame, "ECU1"));
}

void test_security_check_invalid_id_should_fail_spoof(void) {
    hsm_context_t ctx;
    can_frame_t frame = {
        .id = 0x999, // not in whitelist
        .dlc = 2
    };

    TEST_ASSERT_EQUAL(SEC_ERR_SPOOF, security_check_frame(&ctx, &frame, "ECU1"));
}

void test_security_check_mac_failure_should_return_mac_error(void) {
    hsm_context_t ctx;
    can_frame_t frame = {
        .id = CAN_ID_ENGINE,
        .dlc = 2
    };

    mock_hsm_verify_return = 0; // force MAC failure

    TEST_ASSERT_EQUAL(SEC_ERR_MAC, security_check_frame(&ctx, &frame, "ECU1"));
}

void test_security_check_valid_frame_should_return_ok(void) {
    hsm_context_t ctx;
    can_frame_t frame = {
        .id = CAN_ID_ENGINE,
        .dlc = 2
    };

    mock_hsm_verify_return = 1; // MAC OK

    TEST_ASSERT_EQUAL(SEC_OK, security_check_frame(&ctx, &frame, "ECU1"));
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_security_check_null_frame_should_return_dlc_error);
    RUN_TEST(test_security_check_invalid_dlc_should_fail);
    RUN_TEST(test_security_check_invalid_id_should_fail_spoof);
    RUN_TEST(test_security_check_mac_failure_should_return_mac_error);
    RUN_TEST(test_security_check_valid_frame_should_return_ok);

    return UNITY_END();
}
