#pragma once

#include "helpers.h"
#include "ListData.h"

class Overlay : public ListData
{
public:
    Overlay() : ListData(0)
    {
    }

    explicit Overlay(uint8_t newId) : ListData(newId)
    {
    }
};

typedef std::map<uint8_t, Overlay> Overlays;
