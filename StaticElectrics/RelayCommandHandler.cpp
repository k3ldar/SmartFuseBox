#include "RelayCommandHandler.h"

RelayCommandHandler::RelayCommandHandler(SerialCommandManager* commandMgrComputer, SerialCommandManager* commandMgrLink, const int* relayPins, int totalRelays)
    : _commandMgrComputer(commandMgrComputer), _commandMgrLink(commandMgrLink), _relayCount(totalRelays)
{
    _relays = new int[_relayCount];
    memcpy(_relays, relayPins, sizeof(int) * _relayCount);

    _relayStatus = new bool[_relayCount];

    for (int i = 0; i < totalRelays; i++)
        _relayStatus[i] = false;
}

RelayCommandHandler::~RelayCommandHandler()
{
    delete[] _relays;
    delete[] _relayStatus;
}

const String* RelayCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { RelayTurnAllOff, RelayTurnAllOn, RelayRetrieveStates, 
        RelaySetState, RelayStatusGet };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}

void RelayCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    String cmd = command;
    cmd.trim();

    if (cmd == RelayTurnAllOff)
    {
        if (paramCount == 0)
        {
            // Turn all relays OFF
            for (int i = 0; i < _relayCount; i++)
            {
                setRelayStatus(i, false);
            }

			broadcastRelayStatus(cmd);
        }
        else
        {
			sendAckErr(sender, cmd, "Invalid parameters");
        }
    }
    else if (cmd == RelayTurnAllOn)
    {
        if (paramCount == 0)
        {
            for (int i = 0; i < _relayCount; i++)
            {
                setRelayStatus(i, true);
			}
                    
            broadcastRelayStatus(cmd);
        }
        else
		{
            sendAckErr(sender, cmd, "Invalid parameters");
        }
    }
    else if (cmd == RelayRetrieveStates)
    {
        if (paramCount == 0)
        {
            // Retrieve states of all relays
            for (int i = 0; i < _relayCount; i++)
            {
                uint8_t status = getRelayStatus(i);
                StringKeyValue param = { String(i), String(status) };
                broadcastRelayStatus(cmd, &param);
            }

			broadcastRelayStatus(cmd);
        }
        else
        {
            sendAckErr(sender, cmd, "Invalid parameters");
        }
    }
    else if (cmd == RelaySetState)
    {
        if (paramCount == 1)
        {
            int relayIndex = params[0].key.toInt();
            int state = params[0].value.toInt();

            if (relayIndex < 0 || relayIndex >= _relayCount)
            {
                sendAckErr(sender, cmd, "Invalid relay index");
                return;
			}

            setRelayStatus(relayIndex, state > 0);
			broadcastRelayStatus(cmd, &params[0]);
        }
        else
        {
            sendAckErr(sender, cmd, "Invalid parameters");
        }
	}
    else if (cmd == RelayStatusGet)
    {
        if (paramCount == 1)
        {
            int relayIndex = params[0].key.toInt();
            if (relayIndex < 0 || relayIndex >= _relayCount)
            {
                sendAckErr(sender, cmd, "Invalid relay index");
                return;
            }

            uint8_t status = getRelayStatus(relayIndex);
            StringKeyValue param = { String(relayIndex), String(status) };
            broadcastRelayStatus(cmd, &param);
        }
        else
        {
            sendAckErr(sender, cmd, "Invalid parameters");
        }
	}
    else
    {
        sendAckErr(sender, cmd, "Unknown relay command");
    }
}


void RelayCommandHandler::setup()
{
    // Initialize all relay pins
    for (int i = 0; i < _relayCount; i++)
    {
        _relayStatus[i] = false;
        pinMode(_relays[i], OUTPUT);
        digitalWrite(_relays[i], HIGH);
    }
}

void RelayCommandHandler::setRelayStatus(int relayIndex, bool isOn)
{
    if (relayIndex < 0 || relayIndex >= _relayCount)
        return;

    _relayStatus[relayIndex] = isOn;
    digitalWrite(_relays[relayIndex], isOn ? LOW : HIGH);
}

uint8_t RelayCommandHandler::getRelayStatus(int relayIndex) const
{
    if (relayIndex < 0 || relayIndex >= _relayCount)
        return 0xFF;

	return _relayStatus[relayIndex] ? 1 : 0;
}

void RelayCommandHandler::broadcastRelayStatus(const String& cmd, const StringKeyValue* param)
{
    if (_commandMgrLink != nullptr)
    {
        sendAckOk(_commandMgrLink, cmd, param);
    }

    if (_commandMgrComputer != nullptr)
    {
        sendAckOk(_commandMgrComputer, cmd, param);
    }
}
