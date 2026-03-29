#ifndef HSM_H
#define HSM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "can_types.h"
#include "can.h"

#define HSM_KEY_SIZE 32

typedef struct {
    uint8_t master_key[HSM_KEY_SIZE];
    uint8_t comm_key[HSM_KEY_SIZE];
    uint8_t rng_seed_key[HSM_KEY_SIZE];
    // extend with secure boot, firmware update, etc.
} hsm_keys_t;

typedef struct {
    hsm_keys_t keys;
} hsm_context_t;

// Initialize HSM for an ECU (load keys, seed RNG, etc.)
bool hsm_init(hsm_context_t *ctx, const char *ecu_name);

// Generate MAC + CRC for outgoing frame
bool hsm_protect_frame(hsm_context_t *ctx, can_frame_t *frame);

// Verify MAC + CRC for incoming frame
bool hsm_verify_frame(hsm_context_t *ctx, const can_frame_t *frame);

// Simple RNG wrapper
bool hsm_get_random(hsm_context_t *ctx, uint8_t *out, size_t len);

// Verfication MAC
bool hsm_verify_mac(hsm_context_t *ctx, const can_frame_t *frame);

#endif

