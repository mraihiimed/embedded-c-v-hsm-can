#ifndef PROTECTED_MEMORY_H
#define PROTECTED_MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include "can.h"

#define PM_MAX_REGION_SIZE   4096
#define PM_HASH_SIZE         32      // SHA-256
#define PM_SIGNATURE_SIZE    64      // ECDSA or RSA signature placeholder

// ---------------------------------------------------------------------------
// Protected Memory Region
// ---------------------------------------------------------------------------

typedef struct {
    uint8_t  data[PM_MAX_REGION_SIZE];
    uint32_t size;
    uint8_t  hash[PM_HASH_SIZE];          // Integrity hash (SHA-256)
    bool     locked;                      // True after secure boot
} pm_region_t;

// ---------------------------------------------------------------------------
// Secure Boot Metadata
// ---------------------------------------------------------------------------

typedef struct {
    uint8_t firmware_hash[PM_HASH_SIZE];  // Expected firmware hash
    uint8_t signature[PM_SIGNATURE_SIZE]; // Firmware signature
    uint32_t version;                     // Firmware version counter
    bool signature_valid;                 // Result of signature verification
} pm_secure_boot_t;

// ---------------------------------------------------------------------------
// Protected Memory Context (per ECU)
// ---------------------------------------------------------------------------

typedef struct {
    pm_region_t      region;              // Protected memory block
    pm_secure_boot_t boot;                // Secure boot metadata
    bool             boot_ok;             // True if secure boot succeeded
    bool             tamper_detected;     // Set if memory integrity fails
} protected_memory_t;

// ---------------------------------------------------------------------------
// API
// ---------------------------------------------------------------------------

// Initialize protected memory for an ECU (load firmware hash, signature, etc.)
bool pm_init(protected_memory_t *pm,
             const uint8_t *firmware_hash,
             const uint8_t *signature,
             uint32_t version);

// Perform secure boot: verify firmware hash + signature
bool pm_secure_boot(protected_memory_t *pm);

// Lock memory region (no writes allowed after boot)
bool pm_lock(protected_memory_t *pm);

// Write to protected memory (allowed only before lock)
bool pm_write(protected_memory_t *pm, const uint8_t *data, uint32_t size);

// Read from protected memory (always allowed)
bool pm_read(const protected_memory_t *pm, uint8_t *out, uint32_t *size);

// Verify integrity of protected memory region
bool pm_verify_integrity(protected_memory_t *pm);

// Mark tamper event (used by IDS or HSM)
void pm_flag_tamper(protected_memory_t *pm);

#endif

