#pragma once

#include <SerialCommandManager.h>

class InterceptDebugHandler : public ISerialCommandHandler {
private:
    SerialCommandManager* _computerCommandManager;
public:
    InterceptDebugHandler(SerialCommandManager* computerCommandManager)
        : _computerCommandManager(computerCommandManager)
    {
    }

    bool supportsCommand(const String& command) const override
    {
        // This handler intercepts all commands for debugging purposes
        return true;
	}

    bool handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override
    {
        _computerCommandManager->sendCommand(sender->getRawMessage(), "");
		return false; // Indicate that we did not fully handle the command
    }

    const String* supportedCommands(size_t& count) const override
    {
        // Return empty array since we override supportsCommand() to claim all commands
        count = 0;
        return nullptr;
    }
};