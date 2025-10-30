#include "WarningPage.h"


const char PageName[] = "pageWarning";
const char PagePrevious[] = "pageHome";
const char PageNext[] = "pageHome";
const uint8_t ButtonPrevious = 2;
const uint8_t ButtonNext = 3;


WarningPage::WarningPage(Stream* serialPort,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, commandMgrLink, commandMgrComputer)
{

}

void WarningPage::begin()
{
    sendCommand(PageName); // ensure we are on page 0
}

void WarningPage::refresh()
{
    // Update heartbeat mechanism
    updateHeartbeat(millis());
}

// Handle touch events for buttons
void WarningPage::handleTouch(uint8_t compId, uint8_t eventType)
{
    Serial.print("warning page handle touch: compId=");
    Serial.println(compId);

    // Map component ID to button index
    switch (compId)
    {
    case ButtonPrevious:
        sendCommand(PagePrevious);
        break;

    case ButtonNext:
        sendCommand(PageNext);
        break;
    }
}

void WarningPage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    // Call base class first to handle heartbeat ACKs
    BaseBoatPage::handleExternalUpdate(updateType, data);
}

void WarningPage::onConnectionStateChanged(bool connected)
{

}