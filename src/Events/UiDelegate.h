#pragma once

class UiDelegate
{
public:
    virtual void setPage(uint8_t pageId) = 0;
    virtual void setControlSet(uint8_t controlSetId) = 0;
    virtual void displayDetail(uint16_t controlId) = 0;
};
