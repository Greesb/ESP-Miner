#ifndef ECASH_H
#define ECASH_H

#include "cashaddr.h"
#include <stdint.h>

bool ecash_check_encode(const uint8_t *addr, size_t addr_len, AddressType type, char *output, size_t output_len, const char *bech32_hrp);

#endif
