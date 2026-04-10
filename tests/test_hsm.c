#include "unity.h"
#include "hsm.h"
#include "can_types.h"
#include "mock_network.h"
#include "mock_hsm.h"
#include "mock_ids.h"
#include "mock_log.h"

void setUp(void) {}
void tearDown(void) {}

void test_hsm_init_should_generate_keys(void) {
    hsm_context_t ctx;
    TEST_ASSERT_TRUE(hsm_init(&ctx, "ECU1"));

    // Keys must not be all zero
    int zero = 1;
    for (int i = 0; i < HSM_KEY_SIZE; i++) {
        if (ctx.keys.master_key[i] != 0 ||
            ctx.keys.comm_key[i] != 0 ||
            ctx.keys.rng_seed_key[i] != 0) {
            zero = 0;
            break;
        }
    }
    TEST_ASSERT_FALSE(zero);
}

void test_hsm_protect_frame_should_set_crc_mac_and_secured(void) {
    hsm_context_t ctx;
    hsm_init(&ctx, "ECU1");

    can_frame_t frame = {
        .id = 0x123,
        .dlc = 3,
        .data = {0xAA, 0xBB, 0xCC}
    };

    TEST_ASSERT_TRUE(hsm_protect_frame(&ctx, &frame));

    TEST_ASSERT_TRUE(frame.secured);
    TEST_ASSERT_NOT_EQUAL(0, frame.crc);

    // MAC must not be all zero
    int zero = 1;
    for (int i = 0; i < 32; i++) {
        if (frame.mac[i] != 0) {
            zero = 0;
            break;
        }
    }
    TEST_ASSERT_FALSE(zero);
}

void test_hsm_verify_frame_should_accept_valid_frame(void) {
    hsm_context_t ctx;
    hsm_init(&ctx, "ECU1");

    can_frame_t frame = {
        .id = 0x321,
        .dlc = 2,
        .data = {0x11, 0x22}
    };

    hsm_protect_frame(&ctx, &frame);

    TEST_ASSERT_TRUE(hsm_verify_frame(&ctx, &frame));
}

void test_hsm_verify_frame_should_reject_modified_crc(void) {
    hsm_context_t ctx;
    hsm_init(&ctx, "ECU1");

    can_frame_t frame = {
        .id = 0x321,
        .dlc = 2,
        .data = {0x11, 0x22}
    };

    hsm_protect_frame(&ctx, &frame);

    frame.crc ^= 0xFFFFFFFF; // corrupt CRC

    TEST_ASSERT_FALSE(hsm_verify_frame(&ctx, &frame));
}

void test_hsm_verify_frame_should_reject_modified_mac(void) {
    hsm_context_t ctx;
    hsm_init(&ctx, "ECU1");

    can_frame_t frame = {
        .id = 0x321,
        .dlc = 2,
        .data = {0x11, 0x22}
    };

    hsm_protect_frame(&ctx, &frame);

    frame.mac[0] ^= 0xFF; // corrupt MAC

    TEST_ASSERT_FALSE(hsm_verify_frame(&ctx, &frame));
}

void test_hsm_get_random_should_fill_buffer(void) {
    hsm_context_t ctx;
    hsm_init(&ctx, "ECU1");

    uint8_t buf[16] = {0};

    TEST_ASSERT_TRUE(hsm_get_random(&ctx, buf, sizeof(buf)));

    int zero = 1;
    for (int i = 0; i < 16; i++) {
        if (buf[i] != 0) {
            zero = 0;
            break;
        }
    }
    TEST_ASSERT_FALSE(zero);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_hsm_init_should_generate_keys);
    RUN_TEST(test_hsm_protect_frame_should_set_crc_mac_and_secured);
    RUN_TEST(test_hsm_verify_frame_should_accept_valid_frame);
    RUN_TEST(test_hsm_verify_frame_should_reject_modified_crc);
    RUN_TEST(test_hsm_verify_frame_should_reject_modified_mac);
    RUN_TEST(test_hsm_get_random_should_fill_buffer);

    return UNITY_END();
}
