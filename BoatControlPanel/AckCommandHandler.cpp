#include "AckCommandHandler.h"

AckCommandHandler::AckCommandHandler(NextionControl* nextionControl)
    : _nextionControl(nextionControl)
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

void AckCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    // Normalize command
    String cmd = command;
    cmd.trim();

    if (cmd == AckCommand)
    {
        bool sawRefreshHint = false;

        // Parse params looking for relay state updates like "3=1" (relay index 3 -> on)
        for (int i = 0; i < paramCount; ++i)
        {
            String key = params[i].key;
            String val = params[i].value;
            key.trim();
            val.trim();

            if (key.length() == 0)
                continue;

            // Check for heartbeat acknowledgement (F0=ok)
            if (key == "F0" && val.equalsIgnoreCase("ok"))
            {
                // Notify the current page about the heartbeat acknowledgement
                if (_nextionControl)
                {
                    BaseDisplayPage* currentPage = _nextionControl->getCurrentPage();
                    if (currentPage)
                    {
                        // Notify the page using the generic handleExternalUpdate
                        // No additional data needed for heartbeat ack, pass nullptr
                        currentPage->handleExternalUpdate(static_cast<uint8_t>(PageUpdateType::HeartbeatAck), nullptr);
                    }
                }

                if (sender)
                {
                    sender->sendDebug("Heartbeat ACK received", "ACK");
                }
                continue;
            }

            // If we get an Rn=ok (echo for the command) treat it as a hint to refresh UI
            if (key.startsWith("R") && val.equalsIgnoreCase("ok"))
            {
                sawRefreshHint = true;
                continue;
            }

            // Numeric keys indicate relay index (0-based). Value '1' means on, '0' means off.
            if (isAllDigits(key))
            {
                int relayIndex = key.toInt(); // 0-based index

                bool isOn = false;
                if (val == "1" || val.equalsIgnoreCase("on") || val.equalsIgnoreCase("true") || val.equalsIgnoreCase("ok"))
                    isOn = true;

                // Log the parsed state back to the sender for debugging (if available)
                if (sender)
                {
                    sender->sendDebug(String("Parsed ACK relay ") + String(relayIndex + 1) + (isOn ? "=ON" : "=OFF"), "ACK");
                }

                // Notify the current page about the relay state change using the generic update mechanism
                if (_nextionControl)
                {
                    BaseDisplayPage* currentPage = _nextionControl->getCurrentPage();
                    if (currentPage)
                    {
                        // Create the update data
                        RelayStateUpdate update = { static_cast<uint8_t>(relayIndex), isOn };
                        
                        // Notify the page using the generic handleExternalUpdate with integer type
                        currentPage->handleExternalUpdate(static_cast<uint8_t>(PageUpdateType::RelayState), &update);
                    }
                }
            }
            else
            {
                // Unknown key form - log for debugging
                if (sender)
                    sender->sendDebug(String("Unexpected ACK param: ") + key + "=" + val, "ACK");
            }
        }

        // Optional: still refresh if we saw the hint (for other potential UI updates)
        if (sawRefreshHint && _nextionControl)
            _nextionControl->refreshCurrentPage();
    }
    else
    {
        sendAckErr(sender, cmd, "Unknown ACK command");
    }
}

const String* AckCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { AckCommand };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}