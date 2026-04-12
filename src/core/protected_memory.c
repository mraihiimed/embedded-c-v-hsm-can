#include "protected_memory.h"
#include "config.h"
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Dummy SHA-256 (NOT secure, replace with real crypto later)
// ---------------------------------------------------------------------------
static void sha256_dummy(const uint8_t *data, uint32_t size, uint8_t out[32]) {
    uint8_t acc = 0;
    for (uint32_t i = 0; i < size; i++) acc ^= data[i];
    for (int i = 0; i < 32; i++) out[i] = acc ^ (uint8_t)i;
}

// ---------------------------------------------------------------------------
// Dummy signature verification (always returns true for now)
// ---------------------------------------------------------------------------
static bool verify_signature_dummy(const uint8_t *sig, uint32_t version) {
    (void)sig;
    (void)version;
    return true;
}

// ---------------------------------------------------------------------------
// API IMPLEMENTATION
// ---------------------------------------------------------------------------
bool pm_init(protected_memory_t *pm,
             const uint8_t *firmware_hash,
             const uint8_t *signature,
             uint32_t version)
{
    memset(pm, 0, sizeof(*pm));

    memcpy(pm->boot.firmware_hash, firmware_hash, PM_HASH_SIZE);
    memcpy(pm->boot.signature, signature, PM_SIGNATURE_SIZE);
    pm->boot.version = version;

    // DO NOT reset pm->region.data here
    // DO NOT reset pm->region.size here

    return true;
}
/*bool pm_init(protected_memory_t *pm,
             const uint8_t *firmware_hash,
             const uint8_t *signature,
             uint32_t version)
{
    if (!pm) return false;

    memset(pm, 0, sizeof(*pm));

    memcpy(pm->boot.firmware_hash, firmware_hash, PM_HASH_SIZE);
    memcpy(pm->boot.signature, signature, PM_SIGNATURE_SIZE);
    pm->boot.version = version;

    pm->boot_ok = false;
    pm->tamper_detected = false;

    return true;
}*/

bool pm_secure_boot(protected_memory_t *pm) {
    if (!pm) return false;

    // Verify signature
    pm->boot.signature_valid = verify_signature_dummy(pm->boot.signature,
                                                      pm->boot.version);

    if (!pm->boot.signature_valid) {
        log_msg(LOG_ERROR, "[PM] Secure boot failed: invalid signature\n");
        pm->boot_ok = false;
        return false;
    }

    // Verify firmware hash
    uint8_t calc_hash[PM_HASH_SIZE];
    sha256_dummy(pm->region.data, pm->region.size, calc_hash);

    if (memcmp(calc_hash, pm->boot.firmware_hash, PM_HASH_SIZE) != 0) {
        log_msg(LOG_ERROR, "[PM] Secure boot failed: firmware hash mismatch\n");
        pm->boot_ok = false;
        return false;
    }

    pm->boot_ok = true;
    log_msg(LOG_INFO, "[PM] Secure boot OK\n");
    return true;
}

bool pm_lock(protected_memory_t *pm) {
    if (!pm) return false;
    pm->region.locked = true;
    return true;
}

bool pm_write(protected_memory_t *pm, const uint8_t *data, uint32_t size) {
    if (!pm || !data) return false;
    if (pm->region.locked) return false;
    if (size > PM_MAX_REGION_SIZE) return false;

    memcpy(pm->region.data, data, size);
    pm->region.size = size;

    // Update integrity hash
    sha256_dummy(pm->region.data, pm->region.size, pm->region.hash);

    return true;
}

bool pm_read(const protected_memory_t *pm, uint8_t *out, uint32_t *size) {
    if (!pm || !out || !size) return false;

    memcpy(out, pm->region.data, pm->region.size);
    *size = pm->region.size;

    return true;
}

bool pm_verify_integrity(protected_memory_t *pm) {
    if (!pm) return false;

    uint8_t calc_hash[PM_HASH_SIZE];
    sha256_dummy(pm->region.data, pm->region.size, calc_hash);

    if (memcmp(calc_hash, pm->region.hash, PM_HASH_SIZE) != 0) {
        pm->tamper_detected = true;
        log_msg(LOG_ERROR, "[PM] TAMPER DETECTED: memory integrity failed\n");
        return false;
    }

    return true;
}

void pm_flag_tamper(protected_memory_t *pm) {
    if (!pm) return;
    pm->tamper_detected = true;
}

