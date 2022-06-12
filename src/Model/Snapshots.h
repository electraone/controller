#pragma once

#include "Snapshot.h"

class Snapshots
{
public:
    explicit Snapshots(const char *newAppSandbox);
    virtual ~Snapshots() = default;

    void setProjectId(const char *newProjectId);
    void sendList(void);
    void sendSnapshot(uint8_t bankNumber, uint8_t slot);

private:
    void createSnapshotDir(void);
    void
        composeSnapshotFilename(char *buffer, uint8_t bankNumber, uint8_t slot);

    const char *appSandbox;
    const char *projectId;
    char tempSnapshotFilename[MAX_FILENAME_LENGTH + 1];
    char snapshotDbFilename[MAX_FILENAME_LENGTH + 1];
};
