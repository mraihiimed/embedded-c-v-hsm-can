#include "unity.h"
#include "network.h"
#include "mock_net_lowlevel.h"

//#include "mock_network.h"
//#include "mock_hsm.h"
//#include "mock_ids.h"
#include "mock_log.h"

void setUp(void) {
    mock_connect_return = 1;
    mock_send_return = 1;
    mock_recv_return = 1;
}

void tearDown(void) {}

void test_network_init_success(void) {
    mock_connect_return = 5; // fake socket fd

    TEST_ASSERT_EQUAL(0, network_init());
}

void test_network_init_failure(void) {
    mock_connect_return = -1;

    TEST_ASSERT_EQUAL(-1, network_init());
}

void test_network_send_frame_success(void) {
    mock_connect_return = 10;
    network_init();

    can_frame_t frame = { .id = 0x123, .dlc = 2, .data = {0xAA, 0xBB} };

    TEST_ASSERT_EQUAL(0, network_send_frame(&frame));
    TEST_ASSERT_EQUAL(10, mock_last_sockfd);
    TEST_ASSERT_EQUAL_HEX8(0xAA, mock_last_sent_frame.data[0]);
}

void test_network_send_frame_null_should_fail(void) {
    mock_connect_return = 10;
    network_init();

    TEST_ASSERT_EQUAL(-1, network_send_frame(NULL));
}

void test_network_send_frame_not_initialized_should_fail(void) {
    // g_bus_sock stays -1
    TEST_ASSERT_EQUAL(-1, network_send_frame(&(can_frame_t){0}));
}

void test_network_send_frame_lowlevel_failure(void) {
    mock_connect_return = 10;
    network_init();

    mock_send_return = 0; // force failure

    can_frame_t frame = { .id = 0x123 };

    TEST_ASSERT_EQUAL(-1, network_send_frame(&frame));
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_network_init_success);
    RUN_TEST(test_network_init_failure);
    RUN_TEST(test_network_send_frame_success);
    RUN_TEST(test_network_send_frame_null_should_fail);
    RUN_TEST(test_network_send_frame_not_initialized_should_fail);
    RUN_TEST(test_network_send_frame_lowlevel_failure);

    return UNITY_END();
}
