#pragma once

#include "Rule.h"
#include <stdint.h>

struct Response {
    uint8_t id;
    uint16_t length;
    std::vector<uint8_t> headers;
    std::vector<Rule> rules;
};
