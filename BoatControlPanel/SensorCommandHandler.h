#pragma once

#include <Arduino.h>
#include "HomePage.h"
#include "BaseBoatCommandHandler.h"
#include "ConfigManager.h"

const char SensorTemperature[] = "S0";
const char SensorHumidity[] = "S1";
const char SensorBearing[] = "S2";
const char SensorDirection[] = "S3";
const char SensorSpeed[] = "S4";
const char SensorCompassTemp[] = "S5";
const char SensorWaterLevel[] = "S6";
const char SensorWaterPumpActive[] = "S7";
const char SensorHornActive[] = "S8";

class SensorCommandHandler : public BaseBoatCommandHandler
{
public:
    // Constructor: pass the NextionControl pointer so we can notify the current page
    explicit SensorCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager);

    void handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;
    const String* supportedCommands(size_t& count) const override;
};