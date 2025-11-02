#include <Arduino.h>
#include "BaseBoatPage.h"

BaseBoatPage::BaseBoatPage(Stream* serialPort, 
                           WarningManager* warningMgr,
                           SerialCommandManager* commandMgrLink,
                           SerialCommandManager* commandMgrComputer) 
    : BaseDisplayPage(serialPort), 
      _config(nullptr),
      _commandMgrLink(commandMgrLink),
      _commandMgrComputer(commandMgrComputer),
      _warningManager(warningMgr)
{
}

BaseBoatPage::~BaseBoatPage()
{
    // Base destructor - no cleanup needed currently
}

void BaseBoatPage::configSet(Config* config)
{
    _config = config;
}

void BaseBoatPage::configUpdated()
{
    // Default implementation - override in derived classes if needed
}
