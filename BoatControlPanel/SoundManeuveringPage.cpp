#include "SoundManeuveringPage.h"



// Nextion Names/Ids on current Page
constexpr uint8_t BtnStarboard = 2; // b0
constexpr uint8_t BtnAstern = 3; // b1
constexpr uint8_t BtnPort = 4; // b2
constexpr uint8_t BtnDanger = 5; // b3
constexpr uint8_t BtnBack = 6; // b4

constexpr unsigned long RefreshIntervalMs = 10000;


SoundManeuveringPage::SoundManeuveringPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{
}

void SoundManeuveringPage::begin()
{

}

void SoundManeuveringPage::refresh(unsigned long now)
{
    (void)now;
}

// Handle touch events for buttons
void SoundManeuveringPage::handleTouch(uint8_t compId, uint8_t eventType)
{
	(void)eventType;

    switch (compId)
    {
        case BtnStarboard:
            getCommandMgrLink()->sendCommand(SoundSignalMoveStarboard, "");
            break;

        case BtnAstern:
            getCommandMgrLink()->sendCommand(SoundSignalMoveAstern, "");
            break;

        case BtnPort:
            getCommandMgrLink()->sendCommand(SoundSignalMovePort, "");
            break;

        case BtnDanger:
            getCommandMgrLink()->sendCommand(SoundSignalMoveDanger, "");
            break;

        case BtnBack:
            setPage(PageSoundSignals);
            break;

    }
}
