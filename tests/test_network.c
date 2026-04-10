#include "unity.h"
#include "network.h"
#include "mock_net_lowlevel.h"
#include "config.h"

// ---------------------------------------------------------------------------
// Setup / Teardown
// ---------------------------------------------------------------------------

void setUp(void)
{
    network_reset();              // reset static state
    mock_net_lowlevel_reset();    // reset mocks
}

void tearDown(void)
{
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

void test_network_init_success(void)
{
    mock_connect_return = 5;

    int ret = network_init();

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(5, mock_connect_return);
    TEST_ASSERT_EQUAL_STRING(BUS_SERVER_HOST, mock_last_host);
    TEST_ASSERT_EQUAL(BUS_SERVER_PORT, mock_last_port);
}

void test_network_init_failure(void)
{
    mock_connect_return = -1;

    int ret = network_init();

    TEST_ASSERT_EQUAL(-1, ret);
}

void test_network_send_frame_success(void)
{
    mock_connect_return = 10;
    network_init();

    can_frame_t frame = {
        .id = 0x123,
        .dlc = 2,
        .data = {0xAA, 0xBB}
    };

    int ret = network_send_frame(&frame);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(10, mock_last_sockfd);

    TEST_ASSERT_EQUAL_HEX32(0x123, mock_last_sent_frame.id);
    TEST_ASSERT_EQUAL(2, mock_last_sent_frame.dlc);
    TEST_ASSERT_EQUAL_HEX8(0xAA, mock_last_sent_frame.data[0]);
    TEST_ASSERT_EQUAL_HEX8(0xBB, mock_last_sent_frame.data[1]);
}

void test_network_send_frame_null_should_fail(void)
{
    mock_connect_return = 10;
    network_init();

    int ret = network_send_frame(NULL);

    TEST_ASSERT_EQUAL(-1, ret);
}

void test_network_send_frame_not_initialized_should_fail(void)
{
    can_frame_t frame = {0};

    int ret = network_send_frame(&frame);

    TEST_ASSERT_EQUAL(-1, ret);
}

void test_network_send_frame_lowlevel_failure(void)
{
    mock_connect_return = 10;
    network_init();

    mock_send_return = false;

    can_frame_t frame = { .id = 0x123 };

    int ret = network_send_frame(&frame);

    TEST_ASSERT_EQUAL(-1, ret);
}

// ---------------------------------------------------------------------------
// Test Runner
// ---------------------------------------------------------------------------

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_network_init_success);
    RUN_TEST(test_network_init_failure);
    RUN_TEST(test_network_send_frame_success);
    RUN_TEST(test_network_send_frame_null_should_fail);
    RUN_TEST(test_network_send_frame_not_initialized_should_fail);
    RUN_TEST(test_network_send_frame_lowlevel_failure);

    return UNITY_END();
}
