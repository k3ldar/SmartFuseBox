#include <Arduino.h>
#include "BaseBoatPage.h"

BaseBoatPage::BaseBoatPage(Stream* serialPort, 
                           WarningManager* warningMgr,
                           SerialCommandManager* commandMgrLink,
                           SerialCommandManager* commandMgrComputer) 
    : BaseDisplayPage(serialPort), 
      _config(nullptr),
      _commandMgrLink(commandMgrLink),
      _commandMgrComputer(commandMgrComputer),
      _warningManager(warningMgr)
{
}

BaseBoatPage::~BaseBoatPage()
{
    // Base destructor - no cleanup needed currently
}

void BaseBoatPage::configSet(Config* config)
{
    _config = config;
}

void BaseBoatPage::configUpdated()
{
    // Default implementation - override in derived classes if needed
}

uint8_t BaseBoatPage::getButtonColor(uint8_t buttonIndex, bool isOn, uint8_t maxButtons)
{
    Config* config = getConfig();
    if (!config || buttonIndex >= maxButtons)
    {
        // Default: grey off, blue on
        return isOn ? ImageButtonColorBlue : ImageButtonColorGrey;
    }

    if (isOn)
    {
        // Check if a custom color is configured for this button
        uint8_t configuredColor = config->buttonImage[buttonIndex];
        if (configuredColor != ImageButtonColorDefault &&
            configuredColor >= ImageButtonColorBlue &&
            configuredColor <= ImageButtonColorYellow)
        {
            return configuredColor;
        }

        // Default ON color is blue
        return ImageButtonColorBlue;
    }
    else
    {
        // OFF state always uses grey
        return ImageButtonColorGrey;
    }
}