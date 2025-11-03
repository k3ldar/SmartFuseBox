#include "WarningPage.h"

// Nextion Names/Ids on Warning Page
const uint8_t ButtonPrevious = 2;
const uint8_t ButtonNext = 3;


WarningPage::WarningPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{

}

void WarningPage::begin()
{

}

void WarningPage::refresh(unsigned long now)
{
    // Update current warnings
    
}

// Handle touch events for buttons
void WarningPage::handleTouch(uint8_t compId, uint8_t eventType)
{
    // Only handle release events
    if (eventType != EventRelease) {
        return;
    }

    // Map component ID to button index
    switch (compId)
    {
    case ButtonPrevious:
        setPage(PAGE_HOME);
        break;

    case ButtonNext:
        setPage(PAGE_HOME);
        break;
    }
}

void WarningPage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    // Call base class first to handle heartbeat ACKs
    BaseBoatPage::handleExternalUpdate(updateType, data);
}