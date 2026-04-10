#include "unity.h"
#include "util.h"
#include "mock_util_lowlevel.h"
#include <string.h>
#include <unistd.h>
#include "mock_network.h"
#include "mock_hsm.h"
#include "mock_ids.h"
#include "mock_log.h"

void setUp(void) {
    mock_usleep_called = 0;
    mock_usleep_value = 0;
    mock_log_index = 0;
    memset(mock_log_buffer, 0, sizeof(mock_log_buffer));
}

void tearDown(void) {}

void test_util_sleep_ms_zero_should_not_call_usleep(void) {
    util_sleep_ms(0);
    TEST_ASSERT_EQUAL(0, mock_usleep_called);
}

void test_util_sleep_ms_negative_should_not_call_usleep(void) {
    util_sleep_ms(-10);
    TEST_ASSERT_EQUAL(0, mock_usleep_called);
}

void test_util_sleep_ms_positive_should_call_usleep(void) {
    util_sleep_ms(5);
    TEST_ASSERT_EQUAL(1, mock_usleep_called);
    TEST_ASSERT_EQUAL(5000, mock_usleep_value); // 5 ms → 5000 µs
}

void test_log_msg_debug(void) {
    log_msg(LOG_DEBUG, "Hello %d", 1);
    TEST_ASSERT_TRUE(strstr(mock_log_buffer, "[DEBUG] Hello 1") != NULL);
}

void test_log_msg_info(void) {
    log_msg(LOG_INFO, "Test");
    TEST_ASSERT_TRUE(strstr(mock_log_buffer, "[INFO] Test") != NULL);
}

void test_log_msg_warn(void) {
    log_msg(LOG_WARN, "Warn");
    TEST_ASSERT_TRUE(strstr(mock_log_buffer, "[WARN] Warn") != NULL);
}

void test_log_msg_error(void) {
    log_msg(LOG_ERROR, "Err");
    TEST_ASSERT_TRUE(strstr(mock_log_buffer, "[ERROR] Err") != NULL);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_util_sleep_ms_zero_should_not_call_usleep);
    RUN_TEST(test_util_sleep_ms_negative_should_not_call_usleep);
    RUN_TEST(test_util_sleep_ms_positive_should_call_usleep);
    RUN_TEST(test_log_msg_debug);
    RUN_TEST(test_log_msg_info);
    RUN_TEST(test_log_msg_warn);
    RUN_TEST(test_log_msg_error);

    return UNITY_END();
}
