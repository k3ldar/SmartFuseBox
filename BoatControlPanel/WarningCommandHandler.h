#pragma once

#include <Arduino.h>
#include "HomePage.h"
#include "BaseBoatCommandHandler.h"
#include "ConfigManager.h"

const char WarningsActive[] = "W0";
const char WarningsList[] = "W1";
const char WarningStatus[] = "W2";
const char WarningsClear[] = "W3";
const char WarningsAdd[] = "W4";

class WarningCommandHandler : public BaseBoatCommandHandler
{
private:
	bool convertWarningTypeFromString(const String& str, WarningType& outType);
public:
    // Constructor: pass the NextionControl pointer so we can notify the current page
    explicit WarningCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager);

    bool handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;
    const String* supportedCommands(size_t& count) const override;
};