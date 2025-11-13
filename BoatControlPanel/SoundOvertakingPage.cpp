// 
// 
// 

#include "SoundOvertakingPage.h"




// Nextion Names/Ids on current Page
constexpr uint8_t BtnStarboard = 2; // b0
constexpr uint8_t BtnConsent = 5; // b1
constexpr uint8_t BtnPort = 3; // b2
constexpr uint8_t BtnDanger = 4; // b3
constexpr uint8_t BtnBack = 6; // b4

constexpr unsigned long RefreshIntervalMs = 10000;


SoundOvertakingPage::SoundOvertakingPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{
}

void SoundOvertakingPage::begin()
{

}

void SoundOvertakingPage::refresh(unsigned long now)
{

}

// Handle touch events for buttons
void SoundOvertakingPage::handleTouch(uint8_t compId, uint8_t eventType)
{
    switch (compId)
    {
    case BtnStarboard:
        getCommandMgrLink()->sendCommand(SoundSignalOvertakeStarboard, "");
        break;

    case BtnConsent:
        getCommandMgrLink()->sendCommand(SoundSignalOvertakeConsent, "");
        break;

    case BtnPort:
        getCommandMgrLink()->sendCommand(SoundSignalOvertakePort, "");
        break;

    case BtnDanger:
        getCommandMgrLink()->sendCommand(SoundSignalOvertakeDanger, "");
        break;

    case BtnBack:
        setPage(PageSoundSignals);
        break;

    }
}
