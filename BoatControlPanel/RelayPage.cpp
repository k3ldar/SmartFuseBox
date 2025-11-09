#include "RelayPage.h"


// Nextion Names/Ids on current Home Page
const uint8_t Button1 = 4; // b1
const uint8_t Button2 = 5; // b2
const uint8_t Button3 = 6; // b3
const uint8_t Button4 = 7; // b4
const uint8_t Button5 = 8; // b5
const uint8_t Button6 = 9; // b6
const uint8_t Button7 = 10; // b7
const uint8_t Button8 = 11; // b8
const uint8_t ButtonNext = 3;
const uint8_t ButtonPrevious = 2;

const unsigned long RefreshIntervalMs = 10000;


RelayPage::RelayPage(Stream* serialPort,
    WarningManager* warningMgr,
    SerialCommandManager* commandMgrLink,
    SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{

}

void RelayPage::begin()
{
    // If config already supplied before begin, apply it
    if (getConfig())
    {
        configUpdated();
    }

    setPicture("b1", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b2", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b3", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b4", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b5", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b6", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b7", ImageButtonColorGrey + ImageButtonColorOffset);
    setPicture("b8", ImageButtonColorGrey + ImageButtonColorOffset);
}

void RelayPage::onEnterPage()
{
    if (getConfig())
    {
        configUpdated();
    }

    // Request relay states to update button states
    getCommandMgrLink()->sendCommand("R2", "");
    _lastRefreshTime = millis();
}

void RelayPage::refresh(unsigned long now)
{
    // Send R2 command every 10 seconds to refresh relay states
    if (now - _lastRefreshTime >= RefreshIntervalMs)
    {
        getCommandMgrComputer()->sendDebug(F("Sending R2"), F("RelayPage"));
        _lastRefreshTime = now;
        getCommandMgrLink()->sendCommand("R2", "");
    }
}

// Handle touch events for buttons
void RelayPage::handleTouch(uint8_t compId, uint8_t eventType)
{
    // Map component ID to button index
    uint8_t buttonIndex = InvalidButtonIndex;
    switch (compId)
    {
    case Button1:
        buttonIndex = 0;
        break;

    case Button2:
        buttonIndex = 1;
        break;

    case Button3:
        buttonIndex = 2;
        break;

    case Button4:
        buttonIndex = 3;
        break;

    case Button5:
        buttonIndex = 4;
        break;

    case Button6:
        buttonIndex = 5;
        break;

    case Button7:
        buttonIndex = 6;
        break;

    case Button8:
        buttonIndex = 7;
        break;

    case ButtonNext:
        setPage(PageHome);
        return;

    case ButtonPrevious:
        setPage(PageHome);
        return;

    default:
        return;
    }

    Config* config = getConfig();
    // Check if we have a valid config and the button is mapped to a relay
    if (!config || buttonIndex >= ConfigRelayCount)
        return;

    uint8_t relayIndex = _slotToRelay[buttonIndex];

    // Check if this button slot has a valid relay mapping
    if (relayIndex == 0xFF || relayIndex >= ConfigRelayCount)
        return;

    // Get the relay long name from config (for home page display)
    String relayName = String(config->relayLongNames[relayIndex]);

    SerialCommandManager* commandMgrComputer = getCommandMgrComputer();

    if (eventType == EventPress)
    {
        if (commandMgrComputer)
        {
            commandMgrComputer->sendDebug(relayName + String(F(" pressed")), F("RelayPage"));
        }
    }
    else if (eventType == EventRelease)
    {
        if (commandMgrComputer)
        {
            commandMgrComputer->sendDebug(relayName + String(F(" released")), F("RelayPage"));
        }

        // Toggle button state
        _buttonOn[buttonIndex] = !_buttonOn[buttonIndex];

        // Get the appropriate color based on the new state
        uint8_t newColor = getButtonColor(buttonIndex, _buttonOn[buttonIndex], ConfigRelayCount);
        _buttonImage[buttonIndex] = newColor;

        // Update the button appearance
        setPicture("b" + String(buttonIndex + 1), newColor);
        setPicture2("b" + String(buttonIndex + 1), newColor);

        // Send relay command
        String cmd = String(relayIndex) + (_buttonOn[buttonIndex] ? "=1" : "=0");
        SerialCommandManager* commandMgrLink = getCommandMgrLink();
        if (commandMgrLink)
        {
            commandMgrLink->sendCommand("R3", cmd);
        }
    }
}

void RelayPage::handleExternalUpdate(uint8_t updateType, const void* data)
{
	Serial.println("RelayPage::handleExternalUpdate called");
    // Call base class first to handle heartbeat ACKs
    BaseBoatPage::handleExternalUpdate(updateType, data);

    if (updateType == static_cast<uint8_t>(PageUpdateType::RelayState) && data != nullptr)
    {
        const RelayStateUpdate* update = static_cast<const RelayStateUpdate*>(data);

        // Find if this relay is mapped to any button on this page
        for (uint8_t buttonIndex = 0; buttonIndex < ConfigRelayCount; ++buttonIndex)
        {
            if (_slotToRelay[buttonIndex] == update->relayIndex)
            {
                // Update internal state
                _buttonOn[buttonIndex] = update->isOn;

                // Get the appropriate color for the new state
                uint8_t newColor = getButtonColor(buttonIndex, update->isOn, ConfigRelayCount);
                _buttonImage[buttonIndex] = newColor;

                // Update the button appearance on display
                String buttonName = "b" + String(buttonIndex + 1);
                setPicture(buttonName, newColor);
                setPicture2(buttonName, newColor);

                // Log the update for debugging (using Long name)
                SerialCommandManager* commandMgrComputer = getCommandMgrComputer();
                if (commandMgrComputer)
                {
                    Config* config = getConfig();
                    String relayName = config ? String(config->relayLongNames[update->relayIndex]) : String(update->relayIndex);
                    commandMgrComputer->sendDebug(
                        relayName + " state updated to " + (update->isOn ? "ON" : "OFF"),
                        "RelayPage"
                    );
                }

                break;
            }
        }
    }
}

void RelayPage::configUpdated()
{
    Config* config = getConfig();

    if (!config)
    {
        return;
    }

    for (uint8_t button = 0; button < ConfigRelayCount; ++button)
    {
        _slotToRelay[button] = button;

        // Initialize button to OFF state (grey)
        _buttonOn[button] = false;
        _buttonImage[button] = ImageButtonColorGrey + ImageButtonColorOffset;

        setPicture("b" + String(button + 1), ImageButtonColorGrey + ImageButtonColorOffset);

        String longName = String(config->relayLongNames[button]);
        sendText("b" + String(button + 1), longName);
    }
}
