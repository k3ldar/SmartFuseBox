#include "HomePage.h"

HomePage::HomePage(Stream* serialPort, SerialCommandManager* commandMgrLink, SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort), _commandMgrLink(commandMgrLink), _commandMgrComputer(commandMgrComputer)
{
    
}

void HomePage::begin()
{
    sendCommand(PageName); // ensure we are on page 0
    updateTemperature();
    updateHumidity();
    updateBearing();
    updateSpeed();

    setPicture("p2", IMG_BLANK);
	setPicture("b1", IMG_BTN_COLOR_GREY); 
	setPicture("b2", IMG_BTN_COLOR_GREY); 
	setPicture("b3", IMG_BTN_COLOR_GREY); 
    setPicture("b4", IMG_BTN_COLOR_GREY); 
    _dangerControlShown = false;
	_compassTempAboveNorm = 0;

    // If config already supplied before begin, apply it
    if (getConfig())
        configUpdated();
}

void HomePage::refresh()
{
    updateTemperature();
    updateHumidity();
    updateBearing();
    updateSpeed();

    if (_compassTempAboveNorm > 5)
    {
        _dangerControlShown = !_dangerControlShown;

        _commandMgrLink->sendCommand("DNGR", "Blink:" + String(_dangerControlShown));
    }
}

// Handle touch events for buttons
void HomePage::handleTouch(uint8_t compId, uint8_t eventType)
{
	Serial.print("handle touch: compId=");
	Serial.println(compId);
    // Map component ID to button index
    int buttonIndex = -1;
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

        case ButtonNext: 
            sendCommand(PageNext);
            return;

        case ButtonWarning:
            sendCommand(PageWarning);

        default:
            return;
    }

    Config* config = getConfig();
    // Check if we have a valid config and the button is mapped to a relay
    if (!config || buttonIndex < 0 || buttonIndex >= HOME_BUTTONS)
        return;

    uint8_t relayIndex = _slotToRelay[buttonIndex];

    // Check if this button slot has a valid relay mapping
    if (relayIndex == 0xFF || relayIndex >= RELAY_COUNT)
        return;

    // Get the relay name from config
    String relayName = String(config->relayNames[relayIndex]);

    if (eventType == EventPress)
    {
        _commandMgrComputer->sendDebug(relayName + " pressed", "HomePage");
    }
    else if (eventType == EventRelease)
    {
        _commandMgrComputer->sendDebug(relayName + " released", "HomePage");

        // Toggle button state
        _buttonOn[buttonIndex] = !_buttonOn[buttonIndex];

        // Get the appropriate color based on the new state
        uint8_t newColor = getButtonColor(buttonIndex, _buttonOn[buttonIndex]);
        _buttonImage[buttonIndex] = newColor;

        // Update the button appearance
        setPicture("b" + String(buttonIndex + 1), newColor);
        setPicture2("b" + String(buttonIndex + 1), newColor);

        // Send relay command
        String cmd = String(relayIndex) + (_buttonOn[buttonIndex] ? ":ON" : ":OFF");
        _commandMgrLink->sendCommand("R3", cmd);
    }
}

void HomePage::handleText(String text)
{
    (void)text;
    // nothing to handle here
}

void HomePage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    if (updateType == static_cast<uint8_t>(PageUpdateType::RelayState) && data != nullptr)
    {
        const RelayStateUpdate* update = static_cast<const RelayStateUpdate*>(data);

        // Find if this relay is mapped to any button on this page
        for (uint8_t buttonIndex = 0; buttonIndex < HOME_BUTTONS; ++buttonIndex)
        {
            if (_slotToRelay[buttonIndex] == update->relayIndex)
            {
                // Update internal state
                _buttonOn[buttonIndex] = update->isOn;

                // Get the appropriate color for the new state
                uint8_t newColor = getButtonColor(buttonIndex, update->isOn);
                _buttonImage[buttonIndex] = newColor;

                // Update the button appearance on display
                String buttonName = "b" + String(buttonIndex + 1);
                setPicture(buttonName, newColor);
                setPicture2(buttonName, newColor);

                // Log the update for debugging
                if (_commandMgrComputer)
                {
                    Config* config = getConfig();
                    String relayName = config ? String(config->relayNames[update->relayIndex]) : String(update->relayIndex);
                    _commandMgrComputer->sendDebug(
                        relayName + " state updated to " + (update->isOn ? "ON" : "OFF"),
                        "HomePage"
                    );
                }

                break; // Found the button, no need to continue
            }
        }
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::HeartbeatAck))
    {
        // Handle heartbeat acknowledgement
        // The connection is alive - you can update UI elements here if needed
        // For example: clear connection warning indicator, update status icon, etc.
        if (_commandMgrComputer)
        {
            _commandMgrComputer->sendDebug("Heartbeat OK", "HomePage");
        }
        
        // Example: If you have a connection status indicator on the page, you could update it here:
        // setPicture("connectionStatus", IMG_CONNECTED);
        // Or clear any warning state related to connection loss
    }
}

// --- Public setters ---
void HomePage::setTemperature(float tempC)
{
    if (isnan(_lastTemp) || _lastTemp != tempC)
    {
        _lastTemp = tempC;
        updateTemperature();
    }
}

void HomePage::setHumidity(float humPerc)
{
    if (isnan(_lastHumidity) || _lastHumidity != humPerc)
    {
        _lastHumidity = humPerc;
        updateHumidity();
    }
}

void HomePage::setBearing(float dir)
{
    if (_lastBearing != dir)
    {
        _lastBearing = dir;
        updateBearing();
    }
}

void HomePage::setSpeed(float speedKn)
{
    if (isnan(_lastSpeed) || _lastSpeed != speedKn)
    {
        _lastSpeed = speedKn;
        updateSpeed();
    }
}

void HomePage::setDirection(String dir)
{
    if (_lastDirection != dir)
    {
        _lastDirection = dir;
        updateDirection();
    }
}

void HomePage::setCompassTemperature(float tempC)
{
    if (_lastCompassTemp != tempC)
    {
        _lastCompassTemp = tempC;

        if (tempC > CompassTemperatureWarningValue && _compassTempAboveNorm < 10)
        {
            _compassTempAboveNorm++;
        }
        else
        {
            _compassTempAboveNorm = 0;
        }
    }
}

// --- Private update methods ---
void HomePage::updateTemperature()
{
    if (_lastTemp == NAN)
    {
        sendText(ControlTemperatureControl, "--");
        return;
	}

    sendText(ControlTemperatureControl, String(_lastTemp, 1) + String((char)176) + "C"); // one decimal place
}

void HomePage::updateHumidity()
{
    if (_lastHumidity == NAN)
    {
        sendText(ControlHumidityControl, "--");
        return;
    }

    sendText(ControlHumidityControl, String(_lastHumidity, 1) + "%");
}

void HomePage::updateBearing()
{
    if (_lastBearing == NAN)
    {
        sendText(ControlBearingText, "--");
        return;
    }

    sendText(ControlBearingText, String(_lastBearing, 0) + String((char)176) + "C");
}

void HomePage::updateSpeed()
{
    if (_lastSpeed == NAN)
    {
        sendText(ControlSpeed, "--");
        return;
    }

    sendText(ControlSpeed, String(_lastSpeed, 0) + "kn");
}

void HomePage::updateDirection()
{
    sendText(ControlBearingDirection, _lastDirection);
}

void HomePage::configUpdated()
{
    Config* config = getConfig();
    if (!config)
		return;

    // update Nextion with config details

    // Example: apply home page mapping and enabled mask to UI slots
    for (uint8_t button = 0; button < HOME_BUTTONS; ++button)
    {
        uint8_t relayIndex = config->homePageMapping[button];
        if (relayIndex <= 7)
        {
            _slotToRelay[button] = relayIndex;

            // set picture control (button image) - control names in your Nextion might differ
            setPicture("b" + String(button + 1), _buttonImage[button]);

            // Adjust control names to match your Nextion layout:
            sendText(String("b") + String(button + 1), String(config->relayNames[relayIndex]));
        }
        else
        {
            _slotToRelay[button] = 0xFF;
            _buttonOn[button] = false;
            _buttonImage[button] = IMG_BTN_COLOR_GREY;
            setPicture("b" + String(button + 1), _buttonImage[button]);
            sendText(String("btn") + String(button + 1), ""); // clear label
        }
    }

    // Update the boat name
    sendText(ControlBoatName, String(config->boatName)); 
}

uint8_t HomePage::getButtonColor(uint8_t buttonIndex, bool isOn)
{
    Config* config = getConfig();
    if (!config || buttonIndex >= HOME_BUTTONS)
    {
        // Default: grey off, blue on
        return isOn ? IMG_BTN_COLOR_BLUE : IMG_BTN_COLOR_GREY;
    }

    if (isOn)
    {
        // Check if a custom color is configured for this button
        uint8_t configuredColor = config->homePageButtonImage[buttonIndex];
        if (configuredColor != IMG_BTN_COLOR_DEFAULT &&
            configuredColor >= IMG_BTN_COLOR_BLUE &&
            configuredColor <= IMG_BTN_COLOR_YELLOW) {
            return configuredColor;
        }
        // Default ON color is blue
        return IMG_BTN_COLOR_BLUE;
    }
    else
    {
        // OFF state always uses grey
        return IMG_BTN_COLOR_GREY;
    }
}