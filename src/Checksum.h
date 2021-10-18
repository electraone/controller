#pragma once

#include "stdint.h"

enum class ChecksumAlgorithm { UNKNOWN = 0, ROLAND, WALDORF, FRACTALAUDIO };

#define JSON_DOCUMENT_SIZE      25000
#define VARIABLE_DATA           128
#define VARIABLE_PARAMETER      129
#define VARIABLE_DATA_END       130
#define CHECKSUM                131
#define LUAFUNCTION             132


uint8_t calculateChecksum (uint8_t *payload, uint8_t length);
uint8_t calculateChecksum_fractalaudio (uint8_t *payload, uint8_t length);
