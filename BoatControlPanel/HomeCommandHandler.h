#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>

class HomeCommandHandler : public ISerialCommandHandler {
private:
    HomePage* _homePage;
    SerialCommandManager* _linkCommandManager;
    SerialCommandManager* _computerCommandManager;
public:
    HomeCommandHandler(HomePage* homePage, SerialCommandManager* linkCommandManager, SerialCommandManager* computerCommandManager)
        : _homePage(homePage),
          _linkCommandManager(linkCommandManager),
          _computerCommandManager(computerCommandManager)
    {
    }

    void handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override
    {
        _computerCommandManager->sendDebug(sender->getRawMessage(), F("HomeCommandHandler"));

        if (command == "HUM")
        {
        if (paramCount == 1 && _homePage && _computerCommandManager)
        {
            String token = params[0].value;

            if (token.length() == 0)
                token = params[0].key;

            token.trim();
            _homePage->setHumidity(token.toFloat());
        }
        else if (_computerCommandManager)
        {
            _computerCommandManager->sendError("Invalid float param", "HUMIDITY");
        }
        }
        else if (command == "TMP")
        {
            if (paramCount == 1)
            {
                String token = params[0].value;

                if (token.length() == 0)
                token = params[0].key;

                token.trim();
                _homePage->setTemperature(token.toFloat());
                
            }
            else
            {
                _computerCommandManager->sendError("Invalid float param", "TEMPERATURE");
            }
        }
        else if (command == "DNGR")
        {
            if (paramCount == 1)
            {
                String token = params[0].value;

                if (token.length() == 0)
                token = params[0].key;

                token.trim();
                _computerCommandManager->sendCommand(command, params[0].key + ":" + params[0].value);
                
            }
            else
            {
                _computerCommandManager->sendError("Invalid Blink Option", "TEMPERATURE");
            }
        }
    }

    const String* supportedCommands(size_t& count) const override {
        static const String cmds[] = { "HUM", "TMP", "DNGR" };
        count = 3;
        return cmds;
    }
};