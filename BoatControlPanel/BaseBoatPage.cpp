#include <Arduino.h>
#include "BaseBoatPage.h"

// Initialize static warning flag
bool BaseBoatPage::_isWarning = false;

BaseBoatPage::BaseBoatPage(Stream* serialPort) 
    : BaseDisplayPage(serialPort), _config(nullptr) {
}

BaseBoatPage::~BaseBoatPage() {
    // Base destructor - no cleanup needed currently
}

void BaseBoatPage::configSet(Config* config) {
    _config = config;
}

void BaseBoatPage::configUpdated() {
    // Default implementation - override in derived classes if needed
}

bool BaseBoatPage::isWarning() {
    return _isWarning;
}

void BaseBoatPage::setWarning(bool warning) {
    _isWarning = warning;
}
