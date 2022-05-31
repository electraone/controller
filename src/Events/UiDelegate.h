#pragma once

class UiDelegate
{
public:
    // Actions
    virtual void setPage(uint8_t pageId) = 0;
    virtual void setControlSet(uint8_t controlSetId) = 0;
    virtual void openDetail(uint16_t controlId) = 0;
    virtual void lockDetail(void) = 0;
    virtual void closeDetail(void) = 0;
    virtual void openPageSelection(void) = 0;
    virtual void closePageSelection(void) = 0;
    virtual void ping(void) = 0;

    // Getters
    virtual bool isDetailLocked(void) = 0;
    virtual bool isDetailOnTheLeft(void) = 0;
};
