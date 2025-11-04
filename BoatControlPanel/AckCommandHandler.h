#pragma once
#include <Arduino.h>
#include "HomePage.h"
#include "BaseBoatCommandHandler.h"
#include "ConfigManager.h"

const char AckCommand[] = "ACK";

class AckCommandHandler : public BaseBoatCommandHandler
{
public:
    // Constructor: pass the NextionControl pointer so we can notify the current page
    explicit AckCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager);

    void handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;
    const String* supportedCommands(size_t& count) const override;

private:
    // Parameter processing helpers
    bool processHeartbeatAck(SerialCommandManager* sender, const String& key, const String& value);
    bool processRelayAck(SerialCommandManager* sender, const String& key, const String& value);
};