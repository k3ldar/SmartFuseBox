#pragma once
#include <Arduino.h>
#include "Config.h"
#include "ConfigManager.h"
#include "HomePage.h"
#include "BaseCommandHandler.h"

class ConfigCommandHandler : public BaseCommandHandler
{
public:
    // Constructor: pass the HomePage pointer so we can notify UI when saved/updated
    explicit ConfigCommandHandler(HomePage* homePage);

    bool handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;
    const String* supportedCommands(size_t& count) const override;

private:
    HomePage* _homePage;
};