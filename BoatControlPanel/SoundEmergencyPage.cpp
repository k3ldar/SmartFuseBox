
#include "SoundEmergencyPage.h"

// Nextion Names/Ids on current Page
constexpr uint8_t BtnSoS = 2; // b3
constexpr uint8_t BtnBack = 3; // b4

constexpr unsigned long RefreshIntervalMs = 10000;


SoundEmergencyPage::SoundEmergencyPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{
}

void SoundEmergencyPage::begin()
{

}

void SoundEmergencyPage::refresh(unsigned long now)
{
	(void)now;
}

// Handle touch events for buttons
void SoundEmergencyPage::handleTouch(uint8_t compId, uint8_t eventType)
{
	(void)eventType;

    switch (compId)
    {
    case BtnSoS:
        getCommandMgrLink()->sendCommand(SoundSignalSoS, "");
        break;

    case BtnBack:
        setPage(PageSoundSignals);
        break;

    }
}
