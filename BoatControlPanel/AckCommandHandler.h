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
    explicit AckCommandHandler(NextionControl* nextionControl);

    void handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;
    const String* supportedCommands(size_t& count) const override;

private:
    NextionControl* _nextionControl;
};