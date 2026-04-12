#include "unity.h"
#include "protected_memory.h"
#include "mock_log.h"
#include "mock_sha256.h"
#include "mock_network.h"
#include "mock_hsm.h"
#include "mock_ids.h"
#include "mock_log.h"
#include <string.h>


void setUp(void) {}
void tearDown(void) {}

void test_pm_init_should_initialize_structure(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {1,2,3};
    uint8_t sig[PM_SIGNATURE_SIZE] = {9,9,9};

    TEST_ASSERT_TRUE(pm_init(&pm, fw_hash, sig, 42));

    TEST_ASSERT_EQUAL_UINT32(42, pm.boot.version);
    TEST_ASSERT_FALSE(pm.boot_ok);
    TEST_ASSERT_FALSE(pm.tamper_detected);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(fw_hash, pm.boot.firmware_hash, PM_HASH_SIZE);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sig, pm.boot.signature, PM_SIGNATURE_SIZE);
}
/*void test_pm_secure_boot_success(void)
{
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    memset(&pm, 0, sizeof(pm));
    // Prepare a deterministic region
    const uint8_t data[] = { 1, 2, 3, 4 };
    pm.region.size = sizeof(data);
    memcpy(pm.region.data, data, sizeof(data));

    // Compute expected hash using the mock
    sha256_dummy(pm.region.data, pm.region.size, fw_hash);
    //  pm_init(&pm, fw_hash, sig, 1);
    // Initialize protected memory with the expected hash + dummy signature
    TEST_ASSERT_TRUE(pm_init(&pm, fw_hash, sig, 1));

    // Secure boot should succeed because:
    // - signature is always valid (dummy)
    // - hash matches
    TEST_ASSERT_TRUE(pm_secure_boot(&pm));
    TEST_ASSERT_TRUE(pm.boot_ok);
    TEST_ASSERT_FALSE(pm.tamper_detected);
}*/
void test_pm_secure_boot_success(void)
{
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    memset(&pm, 0, sizeof(pm));

    // Step 1: Initialize protected memory (this resets region)
    TEST_ASSERT_TRUE(pm_init(&pm, fw_hash, sig, 1));

    // Step 2: Write deterministic firmware data AFTER pm_init()
    const uint8_t data[] = { 1, 2, 3, 4 };
    pm.region.size = sizeof(data);
    memcpy(pm.region.data, data, sizeof(data));

    // Step 3: Compute expected hash
    sha256_dummy(data, sizeof(data), fw_hash);

    // Step 4: Store expected hash into pm (overwrite init hash)
    memcpy(pm.boot.firmware_hash, fw_hash, PM_HASH_SIZE);

    // Step 5: Secure boot should now succeed
    TEST_ASSERT_TRUE(pm_secure_boot(&pm));
    TEST_ASSERT_TRUE(pm.boot_ok);
    TEST_ASSERT_FALSE(pm.tamper_detected);
}

/*void test_pm_secure_boot_success(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE]={0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    // Prepare region
    uint8_t data[] = {1,2,3,4};
    pm_init(&pm, fw_hash, sig, 1);
    pm_write(&pm, data, sizeof(data));

    // Recompute correct hash
    sha256_dummy(pm.region.data, pm.region.size, fw_hash);
    memcpy(pm.boot.firmware_hash, fw_hash, PM_HASH_SIZE);

    TEST_ASSERT_TRUE(pm_secure_boot(&pm));
    TEST_ASSERT_TRUE(pm.boot_ok);
}*/

void test_pm_secure_boot_hash_mismatch_should_fail(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    pm_init(&pm, fw_hash, sig, 1);

    uint8_t data[] = {1,2,3};
    pm_write(&pm, data, sizeof(data));

    // Wrong hash in boot
    memset(pm.boot.firmware_hash, 0xFF, PM_HASH_SIZE);

    TEST_ASSERT_FALSE(pm_secure_boot(&pm));
    TEST_ASSERT_FALSE(pm.boot_ok);
}

void test_pm_lock_should_prevent_write(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    pm_init(&pm, fw_hash, sig, 1);
    pm_lock(&pm);

    uint8_t data[] = {1,2,3};
    TEST_ASSERT_FALSE(pm_write(&pm, data, sizeof(data)));
}

void test_pm_write_and_read(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    pm_init(&pm, fw_hash, sig, 1);

    uint8_t data[] = {10,20,30};
    TEST_ASSERT_TRUE(pm_write(&pm, data, sizeof(data)));

    uint8_t out[16];
    uint32_t size = 0;

    TEST_ASSERT_TRUE(pm_read(&pm, out, &size));
    TEST_ASSERT_EQUAL_UINT32(3, size);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, out, 3);
}

void test_pm_verify_integrity_success(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    pm_init(&pm, fw_hash, sig, 1);

    uint8_t data[] = {1,2,3};
    pm_write(&pm, data, sizeof(data));

    TEST_ASSERT_TRUE(pm_verify_integrity(&pm));
    TEST_ASSERT_FALSE(pm.tamper_detected);
}

void test_pm_verify_integrity_failure(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    pm_init(&pm, fw_hash, sig, 1);

    uint8_t data[] = {1,2,3};
    pm_write(&pm, data, sizeof(data));

    // Tamper with memory
    pm.region.data[0] ^= 0xFF;

    TEST_ASSERT_FALSE(pm_verify_integrity(&pm));
    TEST_ASSERT_TRUE(pm.tamper_detected);
}

void test_pm_flag_tamper(void) {
    protected_memory_t pm;
    uint8_t fw_hash[PM_HASH_SIZE] = {0};
    uint8_t sig[PM_SIGNATURE_SIZE] = {0};

    pm_init(&pm, fw_hash, sig, 1);

    pm_flag_tamper(&pm);
    TEST_ASSERT_TRUE(pm.tamper_detected);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_pm_init_should_initialize_structure);
    RUN_TEST(test_pm_secure_boot_success);
    RUN_TEST(test_pm_secure_boot_hash_mismatch_should_fail);
    RUN_TEST(test_pm_lock_should_prevent_write);
    RUN_TEST(test_pm_write_and_read);
    RUN_TEST(test_pm_verify_integrity_success);
    RUN_TEST(test_pm_verify_integrity_failure);
    RUN_TEST(test_pm_flag_tamper);

    return UNITY_END();
}
