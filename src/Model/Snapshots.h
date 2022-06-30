#pragma once

#include "Snapshot.h"
#include "LocalFile.h"

class Snapshots
{
public:
    explicit Snapshots(const char *newAppSandbox);
    virtual ~Snapshots() = default;

    void setProjectId(const char *newProjectId);
    void sendList(uint8_t port);
    void sendSnapshot(uint8_t port, uint8_t bankNumber, uint8_t slot);

    void importSnapshot(LocalFile file,
                        const char *projectId,
                        uint8_t bankNumber,
                        uint8_t slot);
    void sendSnapshotMessages(uint8_t bankNumber, uint8_t slot);
    void saveSnapshot(uint8_t bankNumber,
                      uint8_t slot,
                      const char *newName,
                      uint8_t newColour);
    void removeSnapshot(uint8_t bankNumber, uint8_t slot);
    void swapSnapshot(uint8_t sourceBankNumber,
                      uint8_t sourceSlot,
                      uint8_t destBankNumber,
                      uint8_t destSlot);

    void createSnapshotFilename(char *buffer,
                                const char *projectId,
                                uint8_t bankNumber,
                                uint8_t slot);

private:
    void createSnapshotDir(void);
    void
        composeSnapshotFilename(char *buffer, uint8_t bankNumber, uint8_t slot);
    void updateSnapshotDb(uint8_t bankNumber,
                          uint8_t slot,
                          const char *name,
                          uint8_t colour);
    void removeSnapshotDb(uint8_t bankNumber, uint8_t slot);
    void swapSnapshotDb(uint8_t sourceBankNumber,
                        uint8_t sourceSlot,
                        uint8_t destBankNumber,
                        uint8_t destSlot);

    const char *appSandbox;
    const char *projectId;
    char tempSnapshotFilename[MAX_FILENAME_LENGTH + 1];
    char snapshotDbFilename[MAX_FILENAME_LENGTH + 1];
};
