#include "unity.h"
#include "bus.h"
#include "mock_network.h"
#include "can_types.h"

void setUp(void) {
    mock_network_send_called = 0;
}

void tearDown(void) {}

void test_bus_init_should_return_zero(void) {
    int ret = bus_init();
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_bus_send_frame_should_call_network_layer(void) {
    can_frame_t frame = {
        .id = 0x123,
        .dlc = 2,
        .data = {0xAA, 0xBB}
    };

    int ret = bus_send_frame(&frame);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, mock_network_send_called);
    TEST_ASSERT_EQUAL_HEX8(0xAA, mock_last_frame.data[0]);
    TEST_ASSERT_EQUAL_HEX8(0xBB, mock_last_frame.data[1]);
}

void test_bus_send_frame_null_should_return_error(void) {
    int ret = bus_send_frame(NULL);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    TEST_ASSERT_EQUAL_INT(0, mock_network_send_called);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_bus_init_should_return_zero);
    RUN_TEST(test_bus_send_frame_should_call_network_layer);
    RUN_TEST(test_bus_send_frame_null_should_return_error);

    return UNITY_END();
}
