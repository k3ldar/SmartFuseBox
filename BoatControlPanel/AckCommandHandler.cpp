#include "AckCommandHandler.h"

AckCommandHandler::AckCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager)
	: _computerCommandManager(computerCommandManager), _nextionControl(nextionControl), _warningManager(warningManager)
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

void AckCommandHandler::sendDebugMessage(String message)
{
    if (_computerCommandManager)
    {
        _computerCommandManager->sendDebug(message, "AckCommandHandler");
	}
}

void AckCommandHandler::notifyCurrentPage(uint8_t updateType, const void* data)
{
    if (!_nextionControl)
        return;

    BaseDisplayPage* p = _nextionControl->getCurrentPage();
    
    if (!p)
        return;

    p->handleExternalUpdate(updateType, data);
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

bool AckCommandHandler::processRelayAck(SerialCommandManager* sender, const String& key, const String& value)
{

}

void AckCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    String cmd = command;
    cmd.trim();

    // Validate command
    if (cmd != AckCommand)
    {
        sendDebugMessage("Unknown ACK command " + cmd);
        return;
    }

	// the first param indicates what is being acknowledged (F0=ok for heartbeat ack, R2=ok for relay command ack, etc.)

    if (paramCount == 0)
    {
        sendDebugMessage("No parameters in ACK command");
        return;
	}

    String key = params[0].key;
    key.trim();
	String val = params[0].value;
	val.trim();

    if (key == "F0" && val.equalsIgnoreCase("ok"))
    {
        // Heartbeat acknowledgement
        processHeartbeatAck(sender, key, val);
	}
    else if (key == "R2" && val.equalsIgnoreCase("ok") && paramCount == 2)
    {
        if (!isAllDigits(params[1].key) || !isAllDigits(params[1].value))
        {
            sendDebugMessage("invalid parameters in relay ACK");
            return;
        }

		uint8_t relayIndex = params[1].key.toInt();
		bool isOn = parseBooleanValue(params[1].value);
        RelayStateUpdate update = { relayIndex, isOn };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::RelayState), &update);
    }
	else
    {
		sendDebugMessage("Unknown or invalid ACK command");
    }
}

const String* AckCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { AckCommand };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}