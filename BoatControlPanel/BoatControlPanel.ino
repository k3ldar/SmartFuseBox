#include "SensorCommandHandler.h"
#include <Arduino.h>
#include <SerialCommandManager.h>
#include <NextionControl.h>

#include "AckCommandHandler.h"
#include "TLVCompass.h"
#include "HomePage.h"
#include "HomeCommandHandler.h"
#include "WarningPage.h"
#include "Config.h"
#include "ConfigManager.h"
#include "ConfigCommandHandler.h"
#include "WarningManager.h"


#define COMPUTER_SERIAL Serial
#define NEXTION_SERIAL Serial1
#define LINK_SERIAL Serial2


const unsigned long UpdateIntervalMs = 600;
const unsigned long SerialInitTimeoutMs = 300;
const unsigned long HeartbeatIntervalMs = 1000;
const unsigned long HeartbeatTimeoutMs = 3000;

// forward declares
void InitializeSerial(HardwareSerial& serialPort, unsigned long baudRate, bool waitForConnection = false);
void onLinkCommandReceived(SerialCommandManager* mgr);
void onComputerCommandReceived(SerialCommandManager* mgr);

// Compass with smoothing filter size 15
TLVCompass compass(15);

// Serial managers
SerialCommandManager commandMgrComputer(&COMPUTER_SERIAL, onComputerCommandReceived, '\n', ':', '=', 500, 64);
SerialCommandManager commandMgrLink(&LINK_SERIAL, onLinkCommandReceived, '\n', ':', '=', 500, 64);

// Warning manager with heartbeat monitoring
WarningManager warningManager(&commandMgrLink, HeartbeatIntervalMs, HeartbeatTimeoutMs);

// Nextion display setup
HomePage homePage(&NEXTION_SERIAL, &warningManager, &commandMgrLink, &commandMgrComputer);
WarningPage warningPage(&NEXTION_SERIAL, &warningManager, &commandMgrLink, &commandMgrComputer);
BaseDisplayPage* pages[] = { &homePage, &warningPage };
NextionControl nextion(&NEXTION_SERIAL, pages, sizeof(pages) / sizeof(pages[0]));

// link command handlers
SensorCommandHandler sensorCommandHandler(&commandMgrComputer, &nextion, &warningManager);

// computer command handlers
ConfigCommandHandler configHandler(&homePage);

// shared command handlers
AckCommandHandler ackHandler(&commandMgrComputer, &nextion, &warningManager);

// Timers
unsigned long lastUpdate = 0;

void setup()
{
    ISerialCommandHandler* linkHandlers[] = { &ackHandler, &sensorCommandHandler };
    size_t linkHandlerCount = sizeof(linkHandlers) / sizeof(linkHandlers[0]);
    commandMgrLink.registerHandlers(linkHandlers, linkHandlerCount);

    ISerialCommandHandler* computerHandlers[] = { &configHandler, &ackHandler };
    size_t computerHandlerCount = sizeof(computerHandlers) / sizeof(computerHandlers[0]);
    commandMgrComputer.registerHandlers(computerHandlers, computerHandlerCount);

    InitializeSerial(COMPUTER_SERIAL, 115200, true);
    InitializeSerial(NEXTION_SERIAL, 19200);
    InitializeSerial(LINK_SERIAL, 9600, false);

    commandMgrComputer.sendCommand("INIT", "Initializing Boat Control Panel");

    // retrieve config settings

    ConfigManager::begin();

    if (ConfigManager::load())
        homePage.configSet(ConfigManager::getPtr());

    if (!compass.begin())
    {
      commandMgrComputer.sendError("INIT", "Compass Failed");

      while (1)
          delay(100);
    }

    nextion.begin();
    commandMgrComputer.sendCommand("INIT", "Initialized");
}

void loop()
{
    unsigned long now = millis();

    commandMgrComputer.readCommands();
    commandMgrLink.readCommands();

    nextion.update(now);
	warningManager.update(now);

    if (now - lastUpdate >= UpdateIntervalMs)
    {
        lastUpdate = now;

        if (compass.update(now))
        {
            // Only update HomePage if it's the currently active page
            if (nextion.getCurrentPage() == &homePage)
            {
                homePage.setBearing(compass.getHeading());
                homePage.setDirection(compass.getDirection());
                homePage.setSpeed(21);
                homePage.setCompassTemperature(compass.getTemperature());
            }
        }
        else
        {
            commandMgrComputer.sendError("Compass update failed", "COMPASS");
        }
    }
}

void onLinkCommandReceived(SerialCommandManager* mgr)
{
    String cmd = mgr->getCommand();
    commandMgrComputer.sendError("Unknown command: " + cmd, "LINKHANDLER");
}

void onComputerCommandReceived(SerialCommandManager* mgr)
{
    String cmd = mgr->getCommand();
    commandMgrComputer.sendError("Unknown command: " + cmd, "PCHANDLER");
}

void InitializeSerial(HardwareSerial& serialPort, unsigned long baudRate, bool waitForConnection)
{
    serialPort.begin(baudRate);

    if (waitForConnection)
    {
        unsigned long leave = millis() + SerialInitTimeoutMs;

        while (!serialPort && millis() < leave)
            delay(10);
    }
}
