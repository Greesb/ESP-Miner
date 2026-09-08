#ifndef CASHADDR_H
#define CASHADDR_H

#include <stdlib.h>
typedef enum
{
    P2SH,
    P2PKH,
} AddressType;

bool cashaddr_encode(const uint8_t *addr, size_t addr_len, AddressType type, char *output, size_t output_len, const char *bech32_hrp);

#endif
