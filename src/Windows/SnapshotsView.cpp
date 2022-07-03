#include "SnapshotsView.h"
#include "Database.h"

SnapshotsView::SnapshotsView(UiDelegate &newDelegate,
                             const char *newProjectId,
                             uint8_t newBankNumber,
                             Mode newMode)
    : delegate(newDelegate),
      currentProjectId(newProjectId),
      currentBankNumber(newBankNumber),
      mode(newMode),
      loadButton(nullptr),
      loadAndStayButton(nullptr),
      sendCurrentButton(nullptr),
      removeButton(nullptr),
      saveButton(nullptr),
      snapsButton{ nullptr }
{
    setName("Snapshots");
    addActionButtons();
    addSnapshotButtons();
    setMode(newMode);
    setBounds(0, 6, 1024, 570);
}

void SnapshotsView::paint(Graphics &g)
{
    g.fillAll(Colours::black);
    paintTitleBar(g, "Snapshots", getWidth(), 0x0002);
}

void SnapshotsView::resized(void)
{
    for (uint16_t x = 0; x < 6; x++) {
        for (uint16_t y = 0; y < 6; y++) {
            snapsButton[y * 6 + x]->setBounds(
                x * 170 + 10, y * 74 + 125, 150, 55);
        }
    }
}

void SnapshotsView::snapshotRemoved(uint8_t bankNumber, uint8_t slot)
{
    if (bankNumber == currentBankNumber) {
        uint16_t id = slot;
        snapsButton[id]->setUsed(false);
        repaint();
    }
}

void SnapshotsView::snapshotSaved(uint8_t bankNumber,
                                  uint8_t slot,
                                  const char *name,
                                  uint8_t colour)
{
    if (bankNumber == currentBankNumber) {
        uint16_t id = slot;
        snapsButton[id]->setLabel(name);
        snapsButton[id]->setColour(
            ElectraColours::getNumericRgb565Darker(colour));
        snapsButton[id]->setUsed(true);
        repaint();
    }
}

void SnapshotsView::snapshotsSwapped(uint8_t sourceBankNumber,
                                     uint8_t sourceSlot,
                                     uint8_t destBankNumber,
                                     uint8_t destSlot)
{
    updateSnapsButtons();
    repaint();
}

void SnapshotsView::paintTitleBar(Graphics &g,
                                  const char *title,
                                  uint16_t width,
                                  uint32_t colour)
{
    g.setColour(colour);
    g.fillRect(0, 0, width, 25);
    paintIcon(g, 16, 5);
    g.printText(
        40, 8, title, TextStyle::smallTransparent, width, TextAlign::left);
}

void SnapshotsView::paintIcon(Graphics &g, uint16_t x, uint16_t y)
{
    g.setColour(Colours::white);
    g.drawRect(x, y, 10, 13);
    g.fillRect(x + 5, y + 3, 10, 13);
}

ActionButton *SnapshotsView::addButton(uint16_t id,
                                       const char *label,
                                       uint32_t colour,
                                       uint32_t colourActive,
                                       const Rectangle &bounds)
{
    ActionButton *button = new ActionButton();

    if (button) {
        button->setName(label);
        button->setBounds(bounds);
        button->setLabel(label);
        button->setColours(colour, colourActive);
        button->setId(id);
        addAndMakeVisible(button);
    }
    return (button);
}

void SnapshotsView::addActionButtons(void)
{
    loadButton =
        addButton(101, "LOAD", 0x0003, 0x0009, Rectangle(10, 40, 154, 50));
    if (loadButton) {
        loadButton->onClick = [this]() {
            logMessage("SnapshotsView: switch to mode load");
            setMode(load);
            repaint();
            return (true);
        };
    }

    loadAndStayButton = addButton(
        102, "LOAD & STAY", 0x0003, 0x0009, Rectangle(180, 40, 154, 50));
    if (loadAndStayButton) {
        loadAndStayButton->onClick = [this]() {
            logMessage("SnapshotsView: switch to mode loadAndStay");
            setMode(loadAndStay);
            repaint();
            return (true);
        };
    }

    sendCurrentButton = addButton(
        103, "SEND CURRENT", 0x00e0, 0x00e0, Rectangle(435, 40, 154, 50));
    if (sendCurrentButton) {
        sendCurrentButton->onClick = [this]() {
            logMessage("SnapshotsView: send saved snapshot messages");
            delegate.sendAllControls();
            return (false);
        };
    }

    removeButton =
        addButton(104, "REMOVE", 0x1800, 0x4800, Rectangle(690, 40, 154, 50));
    if (removeButton) {
        removeButton->onClick = [this]() {
            logMessage("SnapshotsView: switch to mode remove");
            setMode(remove);
            repaint();
            return (true);
        };
    }

    saveButton = addButton(
        105, "SAVE CURRENT", 0x1800, 0x3800, Rectangle(860, 40, 154, 50));

    if (saveButton) {
        saveButton->onClick = [this]() {
            logMessage("SnapshotsView: switch to mode save");
            setMode(save);
            repaint();
            return (true);
        };
    }
}

void SnapshotsView::setMode(Mode newMode)
{
    mode = newMode;

    loadButton->setSelected(mode == load ? true : false);
    loadAndStayButton->setSelected(mode == loadAndStay ? true : false);
    removeButton->setSelected(mode == remove ? true : false);
    saveButton->setSelected(mode == save ? true : false);
}

void SnapshotsView::addSnapshotButtons(void)
{
    for (uint8_t i = 0; i < 36; i++) {
        snapsButton[i] = new SnapsButton();

        if (snapsButton[i]) {
            auto button = snapsButton[i];
            char programInfo[SnapsButton::maxProgramInfoLength + 1];
            sprintf(programInfo, "%d-%02d", currentBankNumber + 1, i + 1);

            uint16_t id = (currentBankNumber * 36) + i;

            button->setId(id);
            button->setName(programInfo);
            button->setProgramInfo(programInfo);
            button->setBankNumber(currentBankNumber);
            button->setUsed(false);

            button->onClick = [this, button, i]() {
                runAction(button, i);
            };
        }
        addAndMakeVisible(snapsButton[i]);
    }

    updateSnapsButtons();
}

void SnapshotsView::updateSnapsButtons(void)
{
    SnapshotRecord snapRec;
    char snapshotFilename[MAX_FILENAME_LENGTH + 1];

    snprintf(snapshotFilename,
             MAX_FILENAME_LENGTH,
             "ctrlv2/snaps/%s/snapshot.db",
             currentProjectId);
    Database dbSnapshot(snapshotFilename);

    if (!dbSnapshot.open()) {
        logMessage("SnapsWindow::paint: cannot open the snapshot database");
    }

    for (uint8_t i = 0; i < 36; i++) {
        if (snapsButton[i]) {
            uint16_t id = (currentBankNumber * 36) + i;
            auto button = snapsButton[i];

            if (dbSnapshot.select(id, DB_RECORD snapRec)) {
                button->setUsed(true);
                button->setLabel(snapRec.name);
                button->setColour(
                    ElectraColours::getNumericRgb565Darker(snapRec.colour));
            } else {
                button->setUsed(false);
            }
        }
    }
    dbSnapshot.close();
}

void SnapshotsView::runAction(SnapsButton *button, uint8_t slot)
{
    switch (mode) {
        case SnapshotsView::load:
            loadSnapshot(button, slot);
            break;

        case SnapshotsView::loadAndStay:
            loadAndStaySnapshot(button, slot);
            break;

        case SnapshotsView::remove:
            removeSnapshot(slot);
            break;

        case SnapshotsView::save:
            saveSnapshot(button, slot);
            break;

        default:
            break;
    }
}

void SnapshotsView::loadSnapshot(SnapsButton *button, uint8_t slot)
{
    delegate.loadSnapshot(currentProjectId, currentBankNumber, slot);
    delegate.closeSnapshots();
}

void SnapshotsView::loadAndStaySnapshot(SnapsButton *button, uint8_t slot)
{
    delegate.loadSnapshot(currentProjectId, currentBankNumber, slot);
}

void SnapshotsView::removeSnapshot(uint8_t slot)
{
    delegate.removeSnapshot(currentProjectId, currentBankNumber, slot);
}

void SnapshotsView::saveSnapshot(SnapsButton *button, uint8_t slot)
{
    uint8_t colour = slot % 6;
    char name[20 + 1];
    sprintf(name, "%c%d", 65 + slot % 6, slot / 6);
    delegate.saveSnapshot(
        currentProjectId, currentBankNumber, slot, name, colour);
}
