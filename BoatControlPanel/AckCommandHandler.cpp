#include "AckCommandHandler.h"

AckCommandHandler::AckCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager)
    : BaseBoatCommandHandler(computerCommandManager, nextionControl, warningManager)
{
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

bool AckCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    sendDebugMessage("Processing ACK: " + command + " (" + String(paramCount) + " params)", "ACK");
    
    String cmd = command;
    cmd.trim();

    // Validate command
    if (cmd != AckCommand)
    {
        sendDebugMessage("Unknown ACK command " + cmd, "ACK");
        return false;
    }

    // the first param indicates what is being acknowledged (F0=ok for heartbeat ack, R2=ok for relay command ack, etc.)

    if (paramCount == 0)
    {
        sendDebugMessage("No parameters in ACK command", "ACK");
        return false;
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
    else if (key == "R2" && val.equalsIgnoreCase("ok"))
    {
        // Relay state acknowledgement - handle both formats:
        // 1. ACK:R2=ok (just acknowledgement, no relay state - paramCount == 1)
        // 2. ACK:R2=ok:0=0 (acknowledgement with relay state - paramCount == 2)
        
        if (paramCount >= 2)
        {
            // Format: ACK:R2=ok:0=0 (with relay index and state)
            if (!isAllDigits(params[1].key) || !isAllDigits(params[1].value))
            {
                return true;
            }

            uint8_t relayIndex = params[1].key.toInt();
            bool isOn = parseBooleanValue(params[1].value);
            
            RelayStateUpdate update = { relayIndex, isOn };
            notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::RelayState), &update);
        }
    }
	else
    {
		sendDebugMessage("Unknown or invalid ACK: key='" + key + "', val='" + val + "'", "ACK");
    }

    return true;
}

const String* AckCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { AckCommand };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}