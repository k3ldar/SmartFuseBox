#pragma once
#include <Arduino.h>
#include "HomePage.h"
#include "BaseCommandHandler.h"
#include "ConfigManager.h"

const char AckCommand[] = "ACK";

class AckCommandHandler : public BaseCommandHandler
{
public:
    // Constructor: pass the NextionControl pointer so we can notify the current page
    explicit AckCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager);

    void handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;
    const String* supportedCommands(size_t& count) const override;

private:
    SerialCommandManager* _computerCommandManager;
    NextionControl* _nextionControl;
	WarningManager* _warningManager;

    // Parameter processing helpers
    bool processHeartbeatAck(SerialCommandManager* sender, const String& key, const String& value);
	bool processRelayAck(SerialCommandManager* sender, const String& key, const String& value);

    // Parsing utilities
    bool parseBooleanValue(const String& value) const;

    // Notification helper
    void notifyCurrentPage(uint8_t updateType, const void* data);

    // debug helper
    void sendDebugMessage(String message);
};