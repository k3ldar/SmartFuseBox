#include "HomePage.h"


// Nextion Names/Ids on current Home Page
constexpr float CompassTemperatureWarningValue = 35;
constexpr char ControlHumidity[] = "t3";
constexpr char ControlTemperature[] = "t2";
constexpr char ControlBearingText[] = "t6";
constexpr char ControlBearingDirection[] = "t4";
constexpr char ControlSpeed[] = "t5";
constexpr char ControlBoatName[] = "t0";
constexpr char ControlWarning[] = "p2";
constexpr uint8_t Button1 = 1; // b1
constexpr uint8_t Button2 = 2; // b2
constexpr uint8_t Button3 = 3; // b3
constexpr uint8_t Button4 = 4; // b4
constexpr uint8_t ButtonNext = 12;
constexpr uint8_t ButtonWarning = 13;

constexpr unsigned long RefreshIntervalMs = 10000;


HomePage::HomePage(Stream* serialPort,
                   WarningManager* warningMgr,
                   SerialCommandManager* commandMgrLink,
                   SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, warningMgr, commandMgrLink, commandMgrComputer)
{
    
}

void HomePage::begin()
{
    // If config already supplied before begin, apply it
    if (getConfig())
    {
        configUpdated();
    }
    
    setPicture("b1", ImageButtonColorGrey); 
    setPicture("b2", ImageButtonColorGrey); 
    setPicture("b3", ImageButtonColorGrey); 
    setPicture("b4", ImageButtonColorGrey); 
    _compassTempAboveNorm = 0;
}

void HomePage::onEnterPage()
{
    if (getConfig())
    {
        configUpdated();
    }
    
    // Request relay states to update button states
    getCommandMgrLink()->sendCommand("R2", "");
    _lastRefreshTime = millis();
}

void HomePage::refresh(unsigned long now)
{
    // Send R2 command every 10 seconds to refresh relay states
    if (now - _lastRefreshTime >= RefreshIntervalMs)
    {
        getCommandMgrComputer()->sendDebug(F("Sending R2"), F("HomePage"));
        _lastRefreshTime = now;
        getCommandMgrLink()->sendCommand("R2", "");
    }

    updateTemperature();
    updateHumidity();
    updateBearing();
    updateSpeed();
    
    // Update warning display
    WarningManager* warningMgr = getWarningManager();
    if (warningMgr)
    {
        if (warningMgr->hasWarnings())
        {
            setPicture(ControlWarning, ImageWarning);
        }
        else
        {
            setPicture(ControlWarning, ImageBlank);
        }
        
        // Update connection-related displays
        if (warningMgr->isWarningActive(WarningType::ConnectionLost))
        {
            sendText(ControlHumidity, "--");
            sendText(ControlTemperature, "--");
        }
    }
}

// Handle touch events for buttons
void HomePage::handleTouch(uint8_t compId, uint8_t eventType)
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

        case ButtonNext: 
            setPage(PageRelay);
            return;

        case ButtonWarning:
            setPage(PageWarning);
            return;  

        default:
            return;
    }

    Config* config = getConfig();
    // Check if we have a valid config and the button is mapped to a relay
    if (!config || buttonIndex >= ConfigHomeButtons)
        return;

    uint8_t relayIndex = _slotToRelay[buttonIndex];

    // Check if this button slot has a valid relay mapping
    if (relayIndex == 0xFF || relayIndex >= ConfigRelayCount)
        return;

    // Get the relay short name from config (for home page display)
    String relayName = String(config->relayShortNames[relayIndex]);

    SerialCommandManager* commandMgrComputer = getCommandMgrComputer();

    if (eventType == EventPress)
    {
        if (commandMgrComputer)
        {
            commandMgrComputer->sendDebug(relayName + String(F(" pressed")), F("HomePage"));
        }
    }
    else if (eventType == EventRelease)
    {
        if (commandMgrComputer)
        {
            commandMgrComputer->sendDebug(relayName + String(F(" released")), F("HomePage"));
        }

        // Toggle button state
        _buttonOn[buttonIndex] = !_buttonOn[buttonIndex];

        // Get the appropriate color based on the new state
        uint8_t newColor = getButtonColor(buttonIndex, _buttonOn[buttonIndex], ConfigHomeButtons);
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

void HomePage::handleText(String text)
{
    (void)text;
    // nothing to handle here
}

void HomePage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    getCommandMgrComputer()->sendDebug("HomePage::handleExternalUpdate type=" + String(updateType), "HomePage");
    
    // Call base class first to handle heartbeat ACKs
    BaseBoatPage::handleExternalUpdate(updateType, data);

    if (updateType == static_cast<uint8_t>(PageUpdateType::RelayState) && data != nullptr)
    {
        const RelayStateUpdate* update = static_cast<const RelayStateUpdate*>(data);

        // Find if this relay is mapped to any button on this page
        for (uint8_t buttonIndex = 0; buttonIndex < ConfigHomeButtons; ++buttonIndex)
        {
            if (_slotToRelay[buttonIndex] == update->relayIndex)
            {
                
                // Update internal state
                _buttonOn[buttonIndex] = update->isOn;

                // Get the appropriate color for the new state
                uint8_t newColor = getButtonColor(buttonIndex, update->isOn, ConfigHomeButtons);
                _buttonImage[buttonIndex] = newColor;

                // Update the button appearance on display
                String buttonName = "b" + String(buttonIndex + 1);
                setPicture(buttonName, newColor);
                setPicture2(buttonName, newColor);

                // Log the update for debugging (using short name)
                Config* config = getConfig();
                String relayName = config ? String(config->relayShortNames[update->relayIndex]) : String(update->relayIndex);

                break; // Found the button, no need to continue
            }
        }
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::Temperature) && data != nullptr)
    {
        const FloatStateUpdate* update = static_cast<const FloatStateUpdate*>(data);
        setTemperature(update->value);
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::Humidity) && data != nullptr)
    {
        const IntStateUpdate* update = static_cast<const IntStateUpdate*>(data);
        setHumidity(static_cast<float>(update->value));
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::Bearing) && data != nullptr)
    {
        const FloatStateUpdate* update = static_cast<const FloatStateUpdate*>(data);
        setBearing(update->value);
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::Direction) && data != nullptr)
    {
        const CharStateUpdate* update = static_cast<const CharStateUpdate*>(data);
        setDirection(String(update->value));
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::Speed) && data != nullptr)
    {
        const IntStateUpdate* update = static_cast<const IntStateUpdate*>(data);
        setSpeed(static_cast<float>(update->value));
    }
    else if (updateType == static_cast<uint8_t>(PageUpdateType::CompassTemp) && data != nullptr)
    {
        const FloatStateUpdate* update = static_cast<const FloatStateUpdate*>(data);
        setCompassTemperature(update->value);
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

            if (_compassTempAboveNorm > 5)
                getWarningManager()->raiseWarning(WarningType::HighCompassTemperature);
        }
        else
        {
            _compassTempAboveNorm = 0;
            
            getWarningManager()->clearWarning(WarningType::HighCompassTemperature);
        }
    }
}

// --- Private update methods ---
void HomePage::updateTemperature()
{
    if (isnan(_lastTemp))
    {
        sendText(ControlTemperature, F("--"));
        return;
    }

    sendText(ControlTemperature, String(_lastTemp, 1) + String((char)176) + "C"); // one decimal place
}

void HomePage::updateHumidity()
{
    if (isnan(_lastHumidity))
    {
        sendText(ControlHumidity, F("--"));
        return;
    }

    sendText(ControlHumidity, String(_lastHumidity, 1) + "%");
}

void HomePage::updateBearing()
{
    if (isnan(_lastBearing))
    {
        sendText(ControlBearingText, F("--"));
        return;
    }

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d°", (int)_lastBearing);
    sendText(ControlBearingText, buffer);
}

void HomePage::updateSpeed()
{
    if (isnan(_lastSpeed))
    {
        sendText(ControlSpeed, F("--"));
        return;
    }

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%dkn", (int)_lastSpeed);
    sendText(ControlSpeed, buffer);
}

void HomePage::updateDirection()
{
    sendText(ControlBearingDirection, _lastDirection);
}

void HomePage::configUpdated()
{
    Config* config = getConfig();

    if (!config)
    {
        return;
    }

    // update Nextion with config details
    // Example: apply home page mapping and enabled mask to UI slots
    for (uint8_t button = 0; button < ConfigHomeButtons; ++button)
    {
        uint8_t relayIndex = config->homePageMapping[button];
        if (relayIndex <= 7)
        {
            _slotToRelay[button] = relayIndex;

            // set picture control (button image) - control names in your Nextion might differ
            setPicture("b" + String(button + 1), _buttonImage[button]);

            // Use short name for home page display
            String shortName = String(config->relayShortNames[relayIndex]);
            sendText(String("b") + String(button + 1), shortName);
        }
        else
        {
            _slotToRelay[button] = 0xFF;
            _buttonOn[button] = false;
            _buttonImage[button] = ImageButtonColorGrey;
            setPicture("b" + String(button + 1), _buttonImage[button]);
            sendText(String("b") + String(button + 1), ""); 
        }
    }

    // Update the boat name
    String boatName = String(config->boatName);
    sendText(ControlBoatName, boatName);
}
