#include "Snapshots.h"
#include "Database.h"
#include "ParameterMap.h"
#include "Midi/Sysex.h"

Snapshots::Snapshots(const char *newAppSandbox)
    : appSandbox(newAppSandbox), destBankNumber(0), destSlot(0)
{
    copyString(destProjectId, "noProjectId", 20);
    snprintf(tempSnapshotFilename,
             MAX_FILENAME_LENGTH,
             "%s/snapxfer.tmp",
             appSandbox);
}

void Snapshots::initialise(const char *newProjectId)
{
    createSnapshotDir(newProjectId);
    createSnapshotDatabase(newProjectId);
}

void Snapshots::setDestProjectId(const char *newProjectId)
{
    copyString(destProjectId, newProjectId, 20);
}

const char *Snapshots::getDestProjectId(void) const
{
    return (destProjectId);
}

void Snapshots::setDestBankNumber(uint8_t newBankNumber)
{
    destBankNumber = newBankNumber;
}

uint8_t Snapshots::getDestBankNumber(void) const
{
    return (destBankNumber);
}

void Snapshots::setDestSlot(uint8_t newSlot)
{
    destSlot = newSlot;
}

uint8_t Snapshots::getDestSlot(void) const
{
    return (destSlot);
}

void Snapshots::sendList(uint8_t port, const char *projectId)
{
    char dbFile[MAX_FILENAME_LENGTH + 1];

    snprintf(dbFile,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/snapshot.db",
             appSandbox,
             projectId);

    Database dbSnapshot(dbFile);

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

    sendSysExFile(
        port, tempSnapshotFilename, ElectraCommand::Object::SnapshotList);
}

void Snapshots::sendSnapshot(uint8_t port,
                             const char *projectId,
                             uint8_t bankNumber,
                             uint8_t slot)
{
    char snapshotFilename[MAX_FILENAME_LENGTH + 1];

    createSnapshotFilename(snapshotFilename, projectId, bankNumber, slot);

    if (Hardware::sdcard.exists(snapshotFilename)) {
        sendSysExFile(
            port, snapshotFilename, ElectraCommand::Object::FileSnapshot);
    }
}

Snapshot Snapshots::importSnapshot(LocalFile file)
{
    char snapshotFilename[MAX_FILENAME_LENGTH + 1];
    createSnapshotFilename(
        snapshotFilename, destProjectId, destBankNumber, destSlot);

    Snapshot snapshot(file.getFilepath());

    logMessage(
        "importSnapshot: file=%s, projectId=%s, bankNumber=%d, slot=%d, name=%s, colour=%x",
        snapshotFilename,
        destProjectId,
        destBankNumber,
        destSlot,
        snapshot.getName(),
        snapshot.getColour());

    if (!Hardware::sdcard.exists(snapshotFilename)) {
        if (file.rename(snapshotFilename)) {
            updateSnapshotDb(destProjectId,
                             destBankNumber,
                             destSlot,
                             snapshot.getName(),
                             snapshot.getColour());
        }
    }
    return (snapshot);
}

void Snapshots::sendSnapshotMessages(const char *projectId,
                                     uint8_t bankNumber,
                                     uint8_t slot)
{
    char filename[MAX_FILENAME_LENGTH + 1];
    createSnapshotFilename(filename, projectId, bankNumber, slot);
    if (Hardware::sdcard.exists(filename)) {
        parameterMap.load(filename);
    }
}

void Snapshots::saveSnapshot(const char *projectId,
                             uint8_t bankNumber,
                             uint8_t slot,
                             const char *newName,
                             uint8_t newColour)
{
    char filename[MAX_FILENAME_LENGTH + 1];
    createSnapshotFilename(filename, projectId, bankNumber, slot);
    parameterMap.save(filename);
    updateSnapshotDb(projectId, bankNumber, slot, newName, newColour);
}

void Snapshots::updateSnapshot(const char *projectId,
                               uint8_t bankNumber,
                               uint8_t slot,
                               const char *newName,
                               uint8_t newColour)
{
    updateSnapshotDb(projectId, bankNumber, slot, newName, newColour);
}

void Snapshots::removeSnapshot(const char *projectId,
                               uint8_t bankNumber,
                               uint8_t slot)
{
    removeSnapshotDb(projectId, bankNumber, slot);
}

void Snapshots::swapSnapshot(const char *projectId,
                             uint8_t sourceBankNumber,
                             uint8_t sourceSlot,
                             uint8_t destBankNumber,
                             uint8_t destSlot)
{
    swapSnapshotDb(
        projectId, sourceBankNumber, sourceSlot, destBankNumber, destSlot);
}

void Snapshots::updateSnapshotDb(const char *projectId,
                                 uint8_t bankNumber,
                                 uint8_t slot,
                                 const char *name,
                                 uint8_t colour)
{
    char dbFile[MAX_FILENAME_LENGTH + 1];

    snprintf(dbFile,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/snapshot.db",
             appSandbox,
             projectId);
    Database dbSnapshot(dbFile);

    if (!dbSnapshot.open()) {
        logMessage("updateSnapshotDb: cannot open database: file=%s", dbFile);
    }

    uint16_t id = (bankNumber * 36) + slot;

    SnapshotRecord snapRec;

    snapRec.slot = slot;
    snapRec.bankNumber = bankNumber;
    snapRec.colour = colour;
    copyString(snapRec.name, name, MAX_SNAPSHOT_NAME_LENGTH);

    dbSnapshot.update(id, DB_RECORD snapRec);
    dbSnapshot.close();
}

void Snapshots::removeSnapshotDb(const char *projectId,
                                 uint8_t bankNumber,
                                 uint8_t slot)
{
    bool status;
    char snapshotFilename[MAX_FILENAME_LENGTH + 1];

    snprintf(snapshotFilename,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/s%02d%02d.snp",
             appSandbox,
             projectId,
             bankNumber,
             slot);
    if (Hardware::sdcard.exists(snapshotFilename)) {
        status = Hardware::sdcard.deleteFile(snapshotFilename);
        logMessage("removing file %s: %s",
                   snapshotFilename,
                   (status == true) ? "OK" : "fail");
    }

    uint16_t id = (bankNumber * 36) + slot;

    // update database on success
    snprintf(snapshotFilename,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/snapshot.db",
             appSandbox,
             projectId);
    Database dbSnapshot(snapshotFilename); // x

    dbSnapshot.open();
    logMessage(
        "ElectraApp::removeSnapshotDb: removing projectId=%s, bankNumber=%d, slot=%d, id=%d",
        projectId,
        bankNumber,
        slot,
        id);
    dbSnapshot.remove(id);
    dbSnapshot.close();
}

void Snapshots::swapSnapshotDb(const char *projectId,
                               uint8_t sourceBankNumber,
                               uint8_t sourceSlot,
                               uint8_t destBankNumber,
                               uint8_t destSlot)
{
    bool status;

    char sourceFilename[MAX_FILENAME_LENGTH + 1];
    char tmpFilename[MAX_FILENAME_LENGTH + 1];
    char destFilename[MAX_FILENAME_LENGTH + 1];

    createSnapshotFilename(
        sourceFilename, projectId, sourceBankNumber, sourceSlot);
    snprintf(tmpFilename,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/s%02d%02d.tmp",
             appSandbox,
             projectId,
             sourceBankNumber,
             sourceSlot);
    createSnapshotFilename(destFilename, projectId, destBankNumber, destSlot);

    if (Hardware::sdcard.exists(tmpFilename)) {
        status = Hardware::sdcard.deleteFile(tmpFilename);
        logMessage("swapSnapshotDb: removing tmp file %s: %s",
                   tmpFilename,
                   (status == true) ? "OK" : "fail");
    }

    if (!Hardware::sdcard.renameFile(sourceFilename, tmpFilename)) {
        logMessage("swapSnapshotDb: cannot rename %s to %s",
                   sourceFilename,
                   tmpFilename);
    }
    logMessage("swapSnapshotDb: move %s to %s", sourceFilename, tmpFilename);

    if (Hardware::sdcard.exists(destFilename)) {
        if (!Hardware::sdcard.renameFile(destFilename, sourceFilename)) {
            logMessage("cannot rename %s to %s", destFilename, sourceFilename);
        }
        logMessage(
            "swapSnapshotDb: move %s to %s", destFilename, sourceFilename);
    }

    if (!Hardware::sdcard.renameFile(tmpFilename, destFilename)) {
        logMessage("swapSnapshotDb: cannot rename %s to %s",
                   tmpFilename,
                   destFilename);
    }
    logMessage("swapSnapshotDb: move %s to %s", tmpFilename, destFilename);

    // update database on success
    SnapshotRecord sourceRec;
    SnapshotRecord destRec;

    uint16_t sourceId = (sourceBankNumber * 36) + sourceSlot;
    uint16_t destId = (destBankNumber * 36) + destSlot;

    char snapshotFilename[MAX_FILENAME_LENGTH + 1];
    snprintf(snapshotFilename,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/snapshot.db",
             appSandbox,
             projectId);
    Database dbSnapshot(snapshotFilename); // x

    dbSnapshot.open();

    dbSnapshot.select(sourceId, DB_RECORD sourceRec);
    bool destUsed = dbSnapshot.select(destId, DB_RECORD destRec);

    if (destUsed) {
        uint8_t tmpBankNumber = destRec.bankNumber;
        uint8_t tmpSlot = destRec.slot;

        //uint8_t tmpColour = destRec.colour;
        //char tmpName[MAX_SNAPSHOT_NAME_LENGTH + 1];
        //copyString (tmpName, destRec.name, MAX_SNAPSHOT_NAME_LENGTH);

        destRec.bankNumber = sourceRec.bankNumber;
        destRec.slot = sourceRec.slot;
        //destRec.colour = sourceRec.colour;
        //copyString (destRec.name, sourceRec.name, MAX_SNAPSHOT_NAME_LENGTH);

        sourceRec.bankNumber = tmpBankNumber;
        sourceRec.slot = tmpSlot;
        //sourceRec.colour = tmpColour;
        //copyString (sourceRec.name, tmpName, MAX_SNAPSHOT_NAME_LENGTH);

        dbSnapshot.update(sourceId, DB_RECORD destRec);
        dbSnapshot.update(destId, DB_RECORD sourceRec);
    } else {
        dbSnapshot.remove(sourceId);
        sourceRec.bankNumber = destBankNumber;
        sourceRec.slot = destSlot;
        dbSnapshot.update(destId, DB_RECORD sourceRec);
    }

    logMessage(
        "swapSnapshotDb: swapping snaphosts: projectId=%s, sourceId=%d, sourceBankNumber=%d, sourceSlot=%d, destId=%d, destBankNumber=%d, destSlot=%d",
        projectId,
        sourceId,
        sourceBankNumber,
        sourceSlot,
        destId,
        destBankNumber,
        destSlot);
    dbSnapshot.close();
}

void Snapshots::createSnapshotDir(const char *projectId)
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
        } else {
            logMessage("Snapshots::createSnapshotDir: directory created: %s",
                       dirname);
        }
    } else {
        logMessage("Snapshots::createSnapshotDir: directory already exists: %s",
                   dirname);
    }
}

void Snapshots::createSnapshotDatabase(const char *projectId)
{
    char snapshotDbFilename[MAX_FILENAME_LENGTH + 1];
    snprintf(snapshotDbFilename,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/snapshot.db",
             appSandbox,
             projectId);
    Database dbSnapshot(snapshotDbFilename);

    if (!Hardware::sdcard.exists(snapshotDbFilename)) {
        logMessage("ElectraApp::createSnapshotDatabase: creating db file %s",
                   snapshotDbFilename);
        dbSnapshot.create(432, sizeof(SnapshotRecord));
    }
}

void Snapshots::createSnapshotFilename(char *buffer,
                                       const char *newProjectId,
                                       uint8_t newBankNumber,
                                       uint8_t newSlot)
{
    snprintf(buffer,
             MAX_FILENAME_LENGTH,
             "%s/snaps/%s/s%02d%02d.snp",
             appSandbox,
             newProjectId,
             newBankNumber,
             newSlot);
}
