#include "hsm.h"
#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Placeholder CRC32 and HMAC; replace with real implementations.
static uint32_t crc32_compute(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            uint32_t mask = -(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}

static void hmac_sha256_dummy(const uint8_t *key, size_t key_len,
                              const uint8_t *data, size_t data_len,
                              uint8_t *out32) {
    // NOT SECURE – just a deterministic placeholder.
    uint8_t acc = 0;
    for (size_t i = 0; i < key_len; i++) acc ^= key[i];
    for (size_t i = 0; i < data_len; i++) acc ^= data[i];
    for (size_t i = 0; i < 32; i++) out32[i] = acc ^ (uint8_t)i;
}

bool hsm_init(hsm_context_t *ctx, const char *ecu_name) {
    if (!ctx) return false;
    memset(ctx, 0, sizeof(*ctx));

    // Derive keys from ECU name + time (demo only)
    size_t len = strlen(ecu_name);
    for (size_t i = 0; i < HSM_KEY_SIZE; i++) {
        ctx->keys.master_key[i] = (uint8_t)(ecu_name[i % len] + i);
        ctx->keys.comm_key[i]   = (uint8_t)(ecu_name[i % len] ^ i);
        ctx->keys.rng_seed_key[i] = (uint8_t)(ecu_name[i % len] + 2 * i);
    }

    srand((unsigned)time(NULL));
    log_msg(LOG_INFO, "[HSM] Initialized for ECU '%s'\n", ecu_name);
    return true;
}



bool hsm_verify_mac(hsm_context_t *ctx, const can_frame_t *frame)
{
    if (!ctx || !frame) return false;

    uint8_t buf[4 + 1 + 8];
    size_t len = 0;

    buf[len++] = (frame->id >> 24) & 0xFF;
    buf[len++] = (frame->id >> 16) & 0xFF;
    buf[len++] = (frame->id >> 8)  & 0xFF;
    buf[len++] = (frame->id)       & 0xFF;
    buf[len++] = frame->dlc;

    for (uint8_t i = 0; i < frame->dlc && i < 8; i++) {
        buf[len++] = frame->data[i];
    }

    uint8_t mac_calc[32];
    hmac_sha256_dummy(ctx->keys.comm_key, HSM_KEY_SIZE, buf, len, mac_calc);

    if (memcmp(mac_calc, frame->mac, 32) != 0) {
        log_msg(LOG_WARN, "[HSM] MAC mismatch\n");
        return false;
    }

    return true;
}


bool hsm_protect_frame(hsm_context_t *ctx, can_frame_t *frame) {
    if (!ctx || !frame) return false;

    uint8_t buf[4 + 1 + 8];
    size_t len = 0;
    buf[len++] = (frame->id >> 24) & 0xFF;
    buf[len++] = (frame->id >> 16) & 0xFF;
    buf[len++] = (frame->id >> 8)  & 0xFF;
    buf[len++] = (frame->id)       & 0xFF;
    buf[len++] = frame->dlc;
    for (uint8_t i = 0; i < frame->dlc && i < 8; i++) {
        buf[len++] = frame->data[i];
    }

    frame->crc = crc32_compute(buf, len);
    hmac_sha256_dummy(ctx->keys.comm_key, HSM_KEY_SIZE, buf, len, frame->mac);
    frame->secured = true;
    return true;
}

bool hsm_verify_frame(hsm_context_t *ctx, const can_frame_t *frame) {
    if (!ctx || !frame) return false;

    uint8_t buf[4 + 1 + 8];
    size_t len = 0;
    buf[len++] = (frame->id >> 24) & 0xFF;
    buf[len++] = (frame->id >> 16) & 0xFF;
    buf[len++] = (frame->id >> 8)  & 0xFF;
    buf[len++] = (frame->id)       & 0xFF;
    buf[len++] = frame->dlc;
    for (uint8_t i = 0; i < frame->dlc && i < 8; i++) {
        buf[len++] = frame->data[i];
    }

    uint32_t crc = crc32_compute(buf, len);
    if (crc != frame->crc) {
        log_msg(LOG_WARN, "[HSM] CRC mismatch\n");
        return false;
    }

    uint8_t mac_calc[32];
    hmac_sha256_dummy(ctx->keys.comm_key, HSM_KEY_SIZE, buf, len, mac_calc);
    if (memcmp(mac_calc, frame->mac, 32) != 0) {
        log_msg(LOG_WARN, "[HSM] MAC mismatch\n");
        return false;
    }

    return true;
}

bool hsm_get_random(hsm_context_t *ctx, uint8_t *out, size_t len) {
    (void)ctx;
    if (!out) return false;
    for (size_t i = 0; i < len; i++) {
        out[i] = (uint8_t)(rand() & 0xFF);
    }
    return true;
}

