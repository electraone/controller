#include "Snapshots.h"
#include "Database.h"
#include "Midi/Sysex.h"

Snapshots::Snapshots(const char *newAppSandbox)
    : appSandbox(newAppSandbox), projectId("noProjectId")
{
    snprintf(tempSnapshotFilename,
             MAX_FILENAME_LENGTH,
             "%s/snapxfer.tmp",
             appSandbox);
}

void Snapshots::setProjectId(const char *newProjectId)
{
    projectId = newProjectId;
    createSnapshotDir();
    snprintf(snapshotDbFilename,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/snapshot.db",
             appSandbox,
             projectId);
}

void Snapshots::sendList(void)
{
    Database dbSnapshot(snapshotDbFilename);

    if (!dbSnapshot.open()) {
        logMessage("Snapshots::sendList: cannot open the snapshot storage");
        return; // \todo possible leak?
    }

    File snapshotJsonFile = Hardware::sdcard.createOutputStream(
        tempSnapshotFilename, FILE_WRITE | O_CREAT | O_TRUNC);

    if (!snapshotJsonFile) {
        logMessage("Snapshots::sendList: cannot open transfer file");
        return; // \todo leak - database is open
    }

    SnapshotRecord snapRec;
    bool firstRecord = true;
    char buf[256]; // \todo fix buffer overflow issue

    snapshotJsonFile.print("{");
    snapshotJsonFile.print("\"version\":1,");
    snapshotJsonFile.print("\"projectId\":\"");
    snapshotJsonFile.print(projectId);
    snapshotJsonFile.print("\",\"parameters\":[");

    for (uint16_t i = 0; i < dbSnapshot.getNumRecords(); i++) {
        if (dbSnapshot.select(i, DB_RECORD snapRec)) {
            sprintf(
                buf,
                "%s{\"slot\":%d,\"bankNumber\":%d,\"name\":\"%s\",\"color\":\"%s\"}",
                (firstRecord) ? "" : ",",
                snapRec.slot,
                snapRec.bankNumber,
                snapRec.name,
                ElectraColours::translateColour(snapRec.colour));
            snapshotJsonFile.write(buf, strlen(buf));
            firstRecord = false;
        }
    }

    snapshotJsonFile.print("]}");

    snapshotJsonFile.close();
    dbSnapshot.close();

    sendSysExFile(tempSnapshotFilename, ElectraCommand::Object::SnapshotList);
}

void Snapshots::sendSnapshot(uint8_t bankNumber, uint8_t slot)
{
    char snapshotFilename[MAX_FILENAME_LENGTH + 1];

    composeSnapshotFilename(snapshotFilename, bankNumber, slot);

    if (Hardware::sdcard.exists(snapshotFilename)) {
        sendSysExFile(snapshotFilename, ElectraCommand::Object::FileSnapshot);
    }
}

void Snapshots::createSnapshotDir(void)
{
    char dirname[MAX_DIRNAME_LENGTH + 1];

    snprintf(dirname, MAX_DIRNAME_LENGTH, "%s/snaps", appSandbox);

    if (!Hardware::sdcard.exists(dirname)) {
        if (!Hardware::sdcard.createDirectory(dirname)) {
            logMessage(
                "Snapshots::createSnapshotDir: cannot create snaps directory");
        }
    }

    snprintf(dirname, MAX_DIRNAME_LENGTH, "%s/snaps/%s", appSandbox, projectId);

    if (!Hardware::sdcard.exists(dirname)) {
        if (!Hardware::sdcard.createDirectory(dirname)) {
            logMessage(
                "Snapshots::createSnapshotDir: cannot create preset snaphot storage");
        }
    }
}

void Snapshots::composeSnapshotFilename(char *buffer,
                                        uint8_t bankNumber,
                                        uint8_t slot)
{
    snprintf(buffer,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/s%02d%02d.snp",
             appSandbox,
             projectId,
             bankNumber,
             slot);
}
