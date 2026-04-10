// tests/mock_sha256.c
#include <string.h>
#include <stdint.h>

void sha256_dummy(const uint8_t *data, size_t len, uint8_t out[32]) {
    (void)data;
    for (size_t i = 0; i < 32; i++) {
        out[i] = (uint8_t)(i + len);
    }
}
