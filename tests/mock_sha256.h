// tests/mock_sha256.h
#ifndef MOCK_SHA256_H
#define MOCK_SHA256_H

#include <stdint.h>
#include <stddef.h>

void sha256_dummy(const uint8_t *data, size_t len, uint8_t out[32]);

#endif
