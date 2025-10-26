#pragma once
#include "BaseCommandHandler.h"

const char RelayTurnAllOff[] = "R0";
const char RelayTurnAllOn[] = "R1";
const char RelayRetrieveStates[] = "R2";
const char RelaySetState[] = "R3";
const char RelayStatusGet[] = "R4";

// internal message handlers
class RelayCommandHandler : public BaseCommandHandler
{
private:
    bool* _relayStatus;
    int* _relays;
    int _relayCount;
    SerialCommandManager* _commandMgrComputer;
    SerialCommandManager* _commandMgrLink;
public:
    RelayCommandHandler(SerialCommandManager* commandMgrComputer, SerialCommandManager* commandMgrLink, const int* relayPins, int totalRelays);
    ~RelayCommandHandler();
    void handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;

    const String* supportedCommands(size_t& count) const override;

    void setup();
    void setRelayStatus(int relayIndex, bool isOn);
	uint8_t getRelayStatus(int relayIndex) const;
private:
    void broadcastRelayStatus(const String& cmd, const StringKeyValue* param = nullptr);
};
