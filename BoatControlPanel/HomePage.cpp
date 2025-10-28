#include "HomePage.h"

HomePage::HomePage(Stream* serialPort, SerialCommandManager* commandMgrLink, SerialCommandManager* commandMgrComputer)
    : BaseDisplayPage(serialPort), _commandMgrLink(commandMgrLink), _commandMgrComputer(commandMgrComputer)
{
    
}

void HomePage::begin()
{
    sendCommand(PageName); // ensure we are on page 0
    updateTemperature();
    updateHumidity();
    updateBearing();
    updateSpeed();

	setPicture("b1", BTN_COLOR_GREY); 
	setPicture("b2", BTN_COLOR_GREY); 
	setPicture("b3", BTN_COLOR_GREY); 
    setPicture("b4", BTN_COLOR_GREY); 
    _dangerControlShown = false;
	_compassTempAboveNorm = 0;

    // If config already supplied before begin, apply it
    if (_config)
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
    // Map component ID to button index
    int buttonIndex = -1;
    switch (compId)
    {
        case 3: buttonIndex = 0; break; // b1
        case 2: buttonIndex = 1; break; // b2
        case 4: buttonIndex = 2; break; // b3
        case 5: buttonIndex = 3; break; // b4
        default: return;
    }

    // Check if we have a valid config and the button is mapped to a relay
    if (!_config || buttonIndex < 0 || buttonIndex >= HOME_BUTTONS)
        return;

    uint8_t relayIndex = _slotToRelay[buttonIndex];

    // Check if this button slot has a valid relay mapping
    if (relayIndex == 0xFF || relayIndex >= RELAY_COUNT)
        return;

    // Get the relay name from config
    String relayName = String(_config->relayNames[relayIndex]);

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
    if (updateType == static_cast<uint8_t>(HomePageUpdateType::RelayState) && data != nullptr)
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
                    String relayName = _config ? String(_config->relayNames[update->relayIndex]) : String(update->relayIndex);
                    _commandMgrComputer->sendDebug(
                        relayName + " state updated to " + (update->isOn ? "ON" : "OFF"),
                        "HomePage"
                    );
                }

                break; // Found the button, no need to continue
            }
        }
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
    sendText(ControlTemperatureControl, String(_lastTemp, 1) + String((char)176) + "C"); // one decimal place
}

void HomePage::updateHumidity()
{
    sendText(ControlHumidityControl, String(_lastHumidity, 1) + "%");
}

void HomePage::updateBearing()
{
    sendText(ControlBearingText, String(_lastBearing, 0) + String((char)176) + "C");
}

void HomePage::updateSpeed()
{
    sendText(ControlSpeed, String(_lastSpeed, 0) + "kn");
}

void HomePage::updateDirection()
{
    sendText(ControlBearingDirection, _lastDirection);
}

void HomePage::configSet(Config* config)
{
    _config = config;
    configUpdated();
}

void HomePage::configUpdated()
{
    if (!_config)
		return;

    // update Nextion with config details

    // Example: apply home page mapping and enabled mask to UI slots
    for (uint8_t button = 0; button < HOME_BUTTONS; ++button)
    {
        uint8_t relayIndex = _config->homePageMapping[button];
        if (relayIndex <= 7)
        {
            _slotToRelay[button] = relayIndex;

            // set picture control (button image) - control names in your Nextion might differ
            setPicture("b" + String(button + 1), _buttonImage[button]);

            // Adjust control names to match your Nextion layout:
            sendText(String("b") + String(button + 1), String(_config->relayNames[relayIndex]));
        }
        else
        {
            _slotToRelay[button] = 0xFF;
            _buttonOn[button] = false;
            _buttonImage[button] = BTN_COLOR_GREY;
            setPicture("b" + String(button + 1), _buttonImage[button]);
            sendText(String("btn") + String(button + 1), ""); // clear label
        }
    }

    // Update the boat name
    sendText(ControlBoatName, String(_config->boatName)); 
}

uint8_t HomePage::getButtonColor(uint8_t buttonIndex, bool isOn)
{
    if (!_config || buttonIndex >= HOME_BUTTONS)
    {
        // Default: grey off, blue on
        return isOn ? BTN_COLOR_BLUE : BTN_COLOR_GREY;
    }

    if (isOn)
    {
        // Check if a custom color is configured for this button
        uint8_t configuredColor = _config->homePageButtonImage[buttonIndex];
        if (configuredColor != BTN_COLOR_DEFAULT &&
            configuredColor >= BTN_COLOR_BLUE &&
            configuredColor <= BTN_COLOR_YELLOW) {
            return configuredColor;
        }
        // Default ON color is blue
        return BTN_COLOR_BLUE;
    }
    else
    {
        // OFF state always uses grey
        return BTN_COLOR_GREY;
    }
}