#include "SensorCommandHandler.h"

SensorCommandHandler::SensorCommandHandler(SerialCommandManager* computerCommandManager, NextionControl* nextionControl, WarningManager* warningManager)
    : BaseBoatCommandHandler(computerCommandManager, nextionControl, warningManager)
{
}

void SensorCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    String cmd = command;
    cmd.trim();

    // the first param indicates the value (v=23 or v=NNW)

    if (paramCount == 0)
    {
        sendDebugMessage("No parameters in sensor command", "SensorCommandHandler");
        return;
    }

    String key = params[0].key;
    key.trim();
    String val = params[0].value;
    val.trim();

    if (key == "S0")
    {
        // Heartbeat acknowledgement
    }
    else
    {
        sendDebugMessage("Unknown or invalid Sensor command", "SensorCommandHandler");
    }
}

const String* SensorCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { SensorTemperature, SensorHumidity, SensorBearing,
        SensorDirection, SensorSpeed, SensorCompassTemp, SensorWaterLevel,
        SensorWaterPump, SensorHornActive };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}