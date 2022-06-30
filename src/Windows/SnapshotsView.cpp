#include "SnapshotsView.h"
#include "Database.h"

SnapshotsView::SnapshotsView(UiDelegate &newDelegate,
                             const char *newProjectId,
                             uint8_t newBankNumber)
    : delegate(newDelegate),
      projectId(newProjectId),
      bankNumber(newBankNumber),
      mode(load),
      loadButton(nullptr),
      loadAndStayButton(nullptr),
      sendCurrentButton(nullptr),
      removeButton(nullptr),
      saveButton(nullptr),
      snapsButton{ nullptr }
{
    setName("snapshots");
    addActionButtons();
    addSnapshotButtons(bankNumber);
    setMode(load);
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
                x * 170 + 12, y * 74 + 125, 150, 55);
        }
    }
}

void SnapshotsView::snapshotRemoved(uint8_t bankNumber, uint8_t slot)
{
    uint16_t id = bankNumber * 36 + slot;
    snapsButton[id]->setUsed(false);
    repaint();
}

void SnapshotsView::snapshotSaved(uint8_t bankNumber,
                                  uint8_t slot,
                                  const char *name,
                                  uint8_t colour)
{
    uint16_t id = bankNumber * 36 + slot;
    snapsButton[id]->setLabel(name);
    snapsButton[id]->setColour(ElectraColours::getNumericRgb565Darker(colour));
    snapsButton[id]->setUsed(true);
    repaint();
}

void SnapshotsView::snapshotsSwapped(uint8_t sourceBankNumber,
                                     uint8_t sourceSlot,
                                     uint8_t destBankNumber,
                                     uint8_t destSlot)
{
    uint16_t sourceId = sourceBankNumber * 36 + sourceSlot;
    uint16_t destId = destBankNumber * 36 + destSlot;

    SnapsButton tmpButton;

    tmpButton.setUsed(snapsButton[sourceId]->isUsed());
    tmpButton.setLabel(snapsButton[sourceId]->getLabel());
    tmpButton.setColour(snapsButton[sourceId]->getColour());

    snapsButton[sourceId]->setUsed(snapsButton[destId]->isUsed());
    snapsButton[sourceId]->setLabel(snapsButton[destId]->getLabel());
    snapsButton[sourceId]->setColour(snapsButton[destId]->getColour());

    snapsButton[destId]->setUsed(tmpButton.isUsed());
    snapsButton[destId]->setLabel(tmpButton.getLabel());
    snapsButton[destId]->setColour(tmpButton.getColour());

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

void SnapshotsView::addActionButtons(void)
{
    loadButton = new ActionButton();
    loadButton->setBounds(10, 40, 154, 50);
    loadButton->setLabel("LOAD");
    loadButton->setColours(0x0003, 0x0009);
    loadButton->setId(101);
    loadButton->onClick = [this]() {
        logMessage("snapshotActionLambda: load");
        setMode(load);
        repaint();
        return (true);
    };
    addAndMakeVisible(loadButton);

    loadAndStayButton = new ActionButton();
    loadAndStayButton->setBounds(180, 40, 154, 50);
    loadAndStayButton->setLabel("LOAD & STAY");
    loadAndStayButton->setColours(0x0003, 0x0009);
    loadAndStayButton->setId(102);
    loadAndStayButton->onClick = [this]() {
        logMessage("snapshotActionLambda: loadAndStay");
        setMode(loadAndStay);
        repaint();
        return (true);
    };
    addAndMakeVisible(loadAndStayButton);

    sendCurrentButton = new ActionButton();
    sendCurrentButton->setBounds(435, 40, 154, 50);
    sendCurrentButton->setLabel("SEND CURRENT");
    sendCurrentButton->setColour(0x00e0);
    sendCurrentButton->setId(103);
    sendCurrentButton->onClick = [this]() {
        delegate.sendAllControls();
        return (false);
    };
    addAndMakeVisible(sendCurrentButton);

    removeButton = new ActionButton();
    removeButton->setBounds(690, 40, 154, 50);
    removeButton->setLabel("REMOVE");
    removeButton->setColours(0x1800, 0x4800);
    removeButton->setId(104);
    removeButton->onClick = [this]() {
        logMessage("snapshotActionLambda: remove");
        setMode(remove);
        repaint();
        return (true);
    };
    addAndMakeVisible(removeButton);

    saveButton = new ActionButton();
    saveButton->setBounds(860, 40, 154, 50);
    saveButton->setLabel("SAVE CURRENT");
    saveButton->setColours(0x1800, 0x3800);
    saveButton->setId(105);
    saveButton->onClick = [this]() {
        logMessage("snapshotActionLambda: save");
        setMode(save);
        repaint();
        return (true);
    };
    addAndMakeVisible(saveButton);
}

void SnapshotsView::setMode(Mode newMode)
{
    mode = newMode;

    loadButton->setSelected(mode == load ? true : false);
    loadAndStayButton->setSelected(mode == loadAndStay ? true : false);
    removeButton->setSelected(mode == remove ? true : false);
    saveButton->setSelected(mode == save ? true : false);
}

void SnapshotsView::addSnapshotButtons(uint8_t bankNumber)
{
    SnapshotRecord snapRec;
    char snapshotFilename[MAX_FILENAME_LENGTH + 1];

    snprintf(snapshotFilename,
             MAX_FILENAME_LENGTH,
             "ctrlv2/snaps/%s/snapshot.db",
             projectId);
    Database dbSnapshot(snapshotFilename);

    if (!dbSnapshot.open()) {
        logMessage("SnapsWindow::paint: cannot open the snapshot database");
        return;
    }

    for (uint8_t i = 0; i < 36; i++) {
        snapsButton[i] = new SnapsButton();

        if (snapsButton[i]) {
            auto button = snapsButton[i];
            char programInfo[SnapsButton::maxProgramInfoLength + 1];
            sprintf(programInfo, "%d-%02d", bankNumber + 1, i + 1);

            button->setId(i);
            button->setProgramInfo(programInfo);
            button->setBankNumber(bankNumber);

            if (dbSnapshot.select(i, DB_RECORD snapRec)) {
                button->setUsed(true);
                button->setLabel(snapRec.name);
                button->setColour(
                    ElectraColours::getNumericRgb565Darker(snapRec.colour));
            } else {
                button->setUsed(false);
            }

            button->onClick = [this, button]() {
                uint8_t id = button->getId();
                uint8_t bankNumber = id / 36;
                uint8_t slot = id % 36;
                runAction(button, bankNumber, slot);
            };
        }
        addAndMakeVisible(snapsButton[i]);
    }
    dbSnapshot.close();
}

void SnapshotsView::runAction(SnapsButton *button,
                              uint8_t bankNumber,
                              uint8_t slot)
{
    switch (mode) {
        case SnapshotsView::load:
            loadSnapshot(button, bankNumber, slot);
            break;

        case SnapshotsView::loadAndStay:
            loadAndStaySnapshot(button, bankNumber, slot);
            break;

        case SnapshotsView::remove:
            removeSnapshot(bankNumber, slot);
            break;

        case SnapshotsView::save:
            saveSnapshot(button, bankNumber, slot);
            break;

        default:
            break;
    }
}

void SnapshotsView::loadSnapshot(SnapsButton *button,
                                 uint8_t bankNumber,
                                 uint8_t slot)
{
    delegate.loadSnapshot(projectId, bankNumber, slot);
    delegate.closeSnapshots();
}

void SnapshotsView::loadAndStaySnapshot(SnapsButton *button,
                                        uint8_t bankNumber,
                                        uint8_t slot)
{
    delegate.loadSnapshot(projectId, bankNumber, slot);
}

void SnapshotsView::removeSnapshot(uint8_t bankNumber, uint8_t slot)
{
    delegate.removeSnapshot(projectId, bankNumber, slot);
}

void SnapshotsView::saveSnapshot(SnapsButton *button,
                                 uint8_t bankNumber,
                                 uint8_t slot)
{
    uint8_t colour = slot % 6;
    char name[20 + 1];
    sprintf(name, "%c%d", 65 + slot % 6, slot / 6);
    delegate.saveSnapshot(projectId, bankNumber, slot, name, colour);
}
