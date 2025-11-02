#include "AckCommandHandler.h"

AckCommandHandler::AckCommandHandler(NextionControl* nextionControl, WarningManager* warningManager)
	: _nextionControl(nextionControl), _warningManager(warningManager)
{
}

static bool isAllDigits(const String& s)
{
    if (s.length() == 0) return false;
    for (size_t i = 0; i < s.length(); ++i)
    {
        if (!isDigit(s[i]))
            return false;
    }
    return true;
}

bool AckCommandHandler::parseBooleanValue(const String& value) const
{
    return (value == "1" || 
            value.equalsIgnoreCase("on") || 
            value.equalsIgnoreCase("true") || 
            value.equalsIgnoreCase("ok"));
}

void AckCommandHandler::notifyCurrentPage(uint8_t updateType, const void* data)
{
    if (!_nextionControl)
        return;

    BaseDisplayPage* currentPage = _nextionControl->getCurrentPage();
    if (currentPage)
    {
        currentPage->handleExternalUpdate(updateType, data);
    }
}

bool AckCommandHandler::processHeartbeatAck(SerialCommandManager* sender, const String& key, const String& value)
{
    // Check for heartbeat acknowledgement (F0=ok)
    if (key != "F0" || !value.equalsIgnoreCase("ok"))
        return false;

    if (_warningManager)
    {
        // Notify the warning manager to update heartbeat timestamp
        _warningManager->notifyHeartbeatAck();
	}

    // Notify the current page about the heartbeat acknowledgement
    notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::HeartbeatAck), nullptr);

    if (sender)
    {
        sender->sendDebug("Heartbeat ACK received", "ACK");
    }

    return true;
}

bool AckCommandHandler::processRefreshHint(const String& key, const String& value)
{
    // If we get an Rn=ok (echo for the command) treat it as a hint to refresh UI
    return (key.startsWith("R") && value.equalsIgnoreCase("ok"));
}

bool AckCommandHandler::processRelayState(SerialCommandManager* sender, const String& key, const String& value)
{
    // Numeric keys indicate relay index (0-based). Value '1' means on, '0' means off.
    if (!isAllDigits(key))
        return false;

    int relayIndex = key.toInt(); // 0-based index
    bool isOn = parseBooleanValue(value);

    // Log the parsed state back to the sender for debugging (if available)
    if (sender)
    {
        sender->sendDebug(String("Parsed ACK relay ") + String(relayIndex + 1) + (isOn ? "=ON" : "=OFF"), "ACK");
    }

    // Notify the current page about the relay state change
    RelayStateUpdate update = { static_cast<uint8_t>(relayIndex), isOn };
    notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::RelayState), &update);

    return true;
}

void AckCommandHandler::processUnknownParameter(SerialCommandManager* sender, const String& key, const String& value)
{
    if (sender)
    {
        sender->sendDebug(String("Unexpected ACK param: ") + key + "=" + value, "ACK");
    }
}

void AckCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    // Normalize command
    String cmd = command;
    cmd.trim();

    // Validate command
    if (cmd != AckCommand)
    {
        sendAckErr(sender, cmd, "Unknown ACK command");
        return;
    }

    bool sawRefreshHint = false;

    // Parse params looking for relay state updates like "3=1" (relay index 3 -> on)
    for (int i = 0; i < paramCount; ++i)
    {
        String key = params[i].key;
        String val = params[i].value;
        key.trim();
        val.trim();

        // Skip empty keys
        if (key.length() == 0)
            continue;

        // Process parameter types in priority order
        if (processHeartbeatAck(sender, key, val))
            continue;

        if (processRefreshHint(key, val))
        {
            sawRefreshHint = true;
            continue;
        }

        if (processRelayState(sender, key, val))
            continue;

        // Unknown parameter type
        processUnknownParameter(sender, key, val);
    }

    // Optional: still refresh if we saw the hint (for other potential UI updates)
    if (sawRefreshHint && _nextionControl)
    {
        _nextionControl->refreshCurrentPage();
    }
}

const String* AckCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { AckCommand };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}