#include "BaseBoatCommandHandler.h"

BaseBoatCommandHandler::BaseBoatCommandHandler(
    SerialCommandManager* computerCommandManager,
    NextionControl* nextionControl,
    WarningManager* warningManager
)
    : _computerCommandManager(computerCommandManager)
    , _nextionControl(nextionControl)
    , _warningManager(warningManager)
{
}

void BaseBoatCommandHandler::notifyCurrentPage(uint8_t updateType, const void* data)
{
    if (!_nextionControl)
        return;

    BaseDisplayPage* p = _nextionControl->getCurrentPage();
    
    if (!p)
        return;

    p->handleExternalUpdate(updateType, data);
}

void BaseBoatCommandHandler::sendDebugMessage(const String& message, const String& identifier)
{
    if (_computerCommandManager)
    {
        _computerCommandManager->sendDebug(message, identifier);
    }
}

bool BaseBoatCommandHandler::parseBooleanValue(const String& value) const
{
    return (value == "1" || 
            value.equalsIgnoreCase("on") || 
            value.equalsIgnoreCase("true") || 
            value.equalsIgnoreCase("ok"));
}

bool BaseBoatCommandHandler::isAllDigits(const String& s) const
{
    if (s.length() == 0) 
        return false;

    for (size_t i = 0; i < s.length(); ++i)
    {
        if (!isDigit(s[i]))
            return false;
    }

    return true;
}
