#pragma once

#include "Rule.h"
#include <cstdint>

class Response
{
public:
    Response(){};

    virtual ~Response() = default;

    uint8_t id;
    std::vector<uint8_t> headers;
    std::vector<Rule> rules;

private:
};
