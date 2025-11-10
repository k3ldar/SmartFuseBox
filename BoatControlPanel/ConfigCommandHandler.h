#pragma once
#include <Arduino.h>
#include "Config.h"
#include "ConfigManager.h"
#include "HomePage.h"
#include "BaseCommandHandler.h"

constexpr char ConfigSaveSettings[] = "C0";
constexpr char ConfigGetSettings[] = "C1";
constexpr char ConfigResetSettings[] = "C2";
constexpr char ConfigRenameBoat[] = "C3";
constexpr char ConfigRenameRelay[] = "C4";
constexpr char ConfigMapHomeButton[] = "C5";
constexpr char ConfigSetButtonColor[] = "C6";
constexpr char ConfigBoatType[] = "C7";


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