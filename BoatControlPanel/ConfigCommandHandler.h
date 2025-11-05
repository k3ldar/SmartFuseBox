#pragma once
#include <Arduino.h>
#include "Config.h"
#include "ConfigManager.h"
#include "HomePage.h"
#include "BaseCommandHandler.h"

const char ConfigSaveSettings[] = "C0";
const char ConfigGetSettings[] = "C1";
const char ConfigResetSettings[] = "C2";
const char ConfigRenameBoat[] = "C3";
const char ConfigRenameRelay[] = "C4";
const char ConfigMapHomeButton[] = "C5";
const char ConfigSetHomeButtonColor[] = "C6";


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