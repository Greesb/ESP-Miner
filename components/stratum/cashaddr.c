#include "cashaddr.h"
#include "esp_log.h"
#include "utils.h"
#include <stdlib.h>
#include <stddef.h>

static const char *TAG = "cashaddr";

static const int CASHADDR_CHECKSUM_SIZE = 8;

// Charset containing the 32 symbols used in the base32 encoding.
static const char CHARSET[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

static void cashaddr_base32enc(char *output, const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        output[i] = CHARSET[data[i]];
    }
    output[length] = '\0';
}

static uint64_t cashaddr_polymod(uint8_t *data, size_t data_len)
{
    const uint64_t GENERATOR[] = {
        0x98f2bc8e61ULL,
        0x79b76d99e2ULL,
        0xf33e5fb3c4ULL,
        0xae2eabe2a8ULL,
        0x1e4f43e470ULL
    };
    uint64_t checksum = 1;

    for (size_t i = 0; i < data_len; i++) {
        uint64_t value = data[i];
        uint64_t topBits = checksum >> 35;

        checksum = ((checksum & 0x07ffffffffULL) << 5) ^ value;

        for (size_t j = 0; j < 5; j++) {
            if ((topBits >> j) & 1) {
                checksum ^= GENERATOR[j];
            }
        }
    }
    return checksum ^ 1;
}

static void checksum_to_uint5_array(uint8_t output[CASHADDR_CHECKSUM_SIZE], uint64_t checksum)
{
    for (int i = 0; i < CASHADDR_CHECKSUM_SIZE; ++i) {
        // Extract the least significant 5 bits (31 is 11111 in binary and 1f in hexadecimal)
        output[CASHADDR_CHECKSUM_SIZE - 1 - i] = (uint8_t)(checksum & 0x1f);
        // Shift right by 5 bits
        checksum >>= 5;
    }
}


static int8_t cashaddr_get_hash_size_bits(size_t hash_length)
{
    switch (hash_length * 8) {
        case 160: return 0;
        case 192: return 1;
        case 224: return 2;
        case 256: return 3;
        case 320: return 4;
        case 384: return 5;
        case 448: return 6;
        case 512: return 7;
        default: return -1;
    }

}

bool cashaddr_encode(const uint8_t *addr, size_t addr_len, AddressType type, char *output, size_t output_len, const char *bech32_hrp) {
    uint8_t type_bits = (type == P2SH) ? 8 : 0;

    int8_t hash_size_bits = cashaddr_get_hash_size_bits(addr_len);
    if (hash_size_bits < 0) {
        ESP_LOGE(TAG, "Invalid address length: %u", addr_len);
        return false;
    }

    uint8_t *payload_data_8b = malloc(addr_len+1);
    if (!payload_data_8b) {
        ESP_LOGE(TAG, "payload_data_8b malloc failed");
        return false;
    }
    // type_bits + has_size_bits = versionByte
    payload_data_8b[0] = type_bits + hash_size_bits;
    memcpy(payload_data_8b+1, addr, addr_len);

    uint8_t *payload_data_5b = malloc(addr_len*2);
    if (!payload_data_5b) {
        ESP_LOGE(TAG, "payload_data_5b malloc failed");
        return false;
    }

    size_t payload_data_5b_len = 0;
    // convert payload data from 8bits base to 5bits base
    convert_bits(payload_data_5b, &payload_data_5b_len, 5, payload_data_8b, addr_len + 1, 8, 1);

    size_t prefix_len = strlen(bech32_hrp);
    size_t checksum_len = prefix_len + 1 + payload_data_5b_len + CASHADDR_CHECKSUM_SIZE;

    uint8_t *checksum_data = malloc(checksum_len);
    if (!checksum_data) {
        ESP_LOGE(TAG, "checksum malloc failed");
        return false;
    }
    // Copy the hrp as a 5bits base into checksum_data
    for (int i = 0 ; i < prefix_len ; i++) {
        checksum_data[i] = (uint8_t)(bech32_hrp[i]) & 0x1f;
    }
    checksum_data[prefix_len] = 0;
    memcpy(checksum_data+prefix_len+1, payload_data_5b, payload_data_5b_len);
    memset(checksum_data+prefix_len+1+payload_data_5b_len, 0, CASHADDR_CHECKSUM_SIZE);

    uint64_t checksum = cashaddr_polymod(checksum_data, checksum_len);
    uint8_t *checksum_array = malloc(CASHADDR_CHECKSUM_SIZE);
    if (!checksum_array) {
        ESP_LOGE(TAG, "checksum_array malloc failed");
        return false;
    }
    checksum_to_uint5_array(checksum_array, checksum);

    size_t payload_len = payload_data_5b_len + CASHADDR_CHECKSUM_SIZE;
    uint8_t *payload = malloc(payload_len);
    if (!payload) {
        ESP_LOGE(TAG, "payload malloc failed");
        return false;
    }
    // Concat payload_data_5b and checksum_array into payload
    memcpy(payload, payload_data_5b, payload_data_5b_len);
    memcpy(payload+payload_data_5b_len, checksum_array, CASHADDR_CHECKSUM_SIZE);

    // base32 encode the payload into the output
    cashaddr_base32enc(output, payload, payload_len);

    free(payload);
    free(payload_data_5b);
    free(payload_data_8b);
    free(checksum_data);
    free(checksum_array);

    return true;
}
