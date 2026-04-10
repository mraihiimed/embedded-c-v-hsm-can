#include "unity.h"
#include "ids.h"

#include "mock_network.h"
#include "mock_hsm.h"
#include "mock_ids.h"
#include "mock_log.h"


void setUp(void) {}
void tearDown(void) {}

void test_ids_mac_error_threshold(void) {
    ids_counters_t c = {0};

    TEST_ASSERT_EQUAL(IDS_OK, ids_update_on_mac_error(&c));
    TEST_ASSERT_EQUAL(1, c.mac_errors);

    TEST_ASSERT_EQUAL(IDS_OK, ids_update_on_mac_error(&c));
    TEST_ASSERT_EQUAL(2, c.mac_errors);

    // Threshold is 3
    TEST_ASSERT_EQUAL(IDS_FAIL_SAFE, ids_update_on_mac_error(&c));
    TEST_ASSERT_EQUAL(3, c.mac_errors);
}

void test_ids_crc_error_threshold(void) {
    ids_counters_t c = {0};

    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL(IDS_OK, ids_update_on_crc_error(&c));
        TEST_ASSERT_EQUAL(i + 1, c.crc_errors);
    }

    // 5th error triggers fail-safe
    TEST_ASSERT_EQUAL(IDS_FAIL_SAFE, ids_update_on_crc_error(&c));
    TEST_ASSERT_EQUAL(5, c.crc_errors);
}

void test_ids_unsecured_frame_threshold(void) {
    ids_counters_t c = {0};

    // Threshold is 1 → immediate fail-safe
    TEST_ASSERT_EQUAL(IDS_FAIL_SAFE, ids_update_on_unsecured(&c));
    TEST_ASSERT_EQUAL(1, c.unsecured_frames);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ids_mac_error_threshold);
    RUN_TEST(test_ids_crc_error_threshold);
    RUN_TEST(test_ids_unsecured_frame_threshold);

    return UNITY_END();
}
