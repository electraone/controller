#include "Checksum.h"

uint8_t calculateChecksum(uint8_t *payload, uint8_t length)
{
    uint32_t sum = 0;
    uint8_t checksum = 0;

    for (uint8_t i = 0; i < length; i++) {
        sum += payload[i];
    }
    checksum = (128 - (sum % 128)) & 0x7F;

    return (checksum);
}

uint8_t calculateChecksum_fractalaudio(uint8_t *payload, uint8_t length)
{
    uint8_t checksum = 0xF0;

    for (uint8_t i = 0; i < length; i++) {
        checksum ^= payload[i];
    }
    checksum &= 0x7F;
    return (checksum);
}

uint8_t calculateChecksum_waldorf(uint8_t *payload, uint8_t length)
{
    uint8_t checksum = 0x7F;
    return (checksum);
}
