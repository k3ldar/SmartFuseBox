#include "SoundFogPage.h"

// Nextion Names/Ids on current Page
constexpr uint8_t BtnFogHorn = 4; // b3
constexpr uint8_t BtnBack = 5; // b4

constexpr unsigned long RefreshIntervalMs = 10000;


SoundFogPage::SoundFogPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{
}

void SoundFogPage::begin()
{

}

void SoundFogPage::refresh(unsigned long now)
{
	(void)now;
}

// Handle touch events for buttons
void SoundFogPage::handleTouch(uint8_t compId, uint8_t eventType)
{
	(void)eventType;

    switch (compId)
    {
    case BtnFogHorn:
        getCommandMgrLink()->sendCommand(SoundSignalFog, "");
        break;

    case BtnBack:
        setPage(PageSoundSignals);
        break;

    }
}
