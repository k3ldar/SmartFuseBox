
#include "SoundOtherPage.h"

// Nextion Names/Ids on current Page
constexpr uint8_t BtnSoS = 3; // b3
constexpr uint8_t BtnBack = 2; // b4

constexpr unsigned long RefreshIntervalMs = 10000;


SoundOtherPage::SoundOtherPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{
}

void SoundOtherPage::begin()
{

}

void SoundOtherPage::refresh(unsigned long now)
{
	(void)now;
}

// Handle touch events for buttons
void SoundOtherPage::handleTouch(uint8_t compId, uint8_t eventType)
{
	(void)eventType;

    switch (compId)
    {
    case BtnSoS:
        getCommandMgrLink()->sendCommand(SoundSignalTest, "");
        break;

    case BtnBack:
        setPage(PageSoundSignals);
        break;

    }
}
