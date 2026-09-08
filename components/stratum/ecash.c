#include "cashaddr.h"
#include <stdio.h>
#include <string.h>


static bool is_ecash_hrp(const char *bech32_hrp) {
    return (strncmp(bech32_hrp, "ecash", 5) == 0) || (strncmp(bech32_hrp, "ectest", 6) == 0);
}

bool ecash_check_encode(const uint8_t *addr, size_t addr_len, AddressType type, char *output, size_t output_len, const char *bech32_hrp) {
    if (!is_ecash_hrp(bech32_hrp)) return false;

    snprintf(output, 7, "ecash:");
    switch (type) {
        case P2PKH:
            return cashaddr_encode(addr+3, 20, type, output+6, output_len-6, bech32_hrp);
        case P2SH:
            return cashaddr_encode(addr+2, 20, type, output+6, output_len-6, bech32_hrp);
    }
    return false;
}
