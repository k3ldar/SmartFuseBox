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

    if (key == SensorTemperature)
    {
        FloatStateUpdate update = { val.toFloat() };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::Temperature), &update);
    }
    else if (key = SensorHumidity)
    {
        IntStateUpdate update = { val.toInt() };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::Humidity), &update);
    }
    else if (key = SensorBearing)
    {
        FloatStateUpdate update = { val.toFloat() };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::Bearing), &update);
    }
    else if (key = SensorDirection)
    {
        CharStateUpdate update = {};
        update.length = min(val.length(), (unsigned int)(CharStateUpdate::MAX_LENGTH - 1));
        val.toCharArray(update.value, CharStateUpdate::MAX_LENGTH);
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::Direction), &update);
    }
    else if (key = SensorSpeed)
    {
        IntStateUpdate update = { val.toInt() };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::Speed), &update);
    }
    else if (key == SensorCompassTemp)
    {
        FloatStateUpdate update = { val.toFloat() };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::CompassTemp), &update);
    }
    else if (key = SensorWaterLevel)
    {
        IntStateUpdate update = { val.toInt() };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::WaterLevel), &update);
    }
    else if (key = SensorWaterPumpActive)
    {
        BoolStateUpdate update = { val.toInt() > 0 };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::WaterPumpActive), &update);
    }
    else if (key = SensorHornActive)
    {
        BoolStateUpdate update = { val.toInt() > 0 };
        notifyCurrentPage(static_cast<uint8_t>(PageUpdateType::SensorHornActive), &update);
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
        SensorWaterPumpActive, SensorHornActive };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}