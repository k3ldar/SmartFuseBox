#include <Arduino.h>
#include "BaseBoatPage.h"


// Initialize static warning flag
bool BaseBoatPage::_isWarning = false;

BaseBoatPage::BaseBoatPage(Stream* serialPort, 
                           SerialCommandManager* commandMgrLink,
                           SerialCommandManager* commandMgrComputer) 
    : BaseDisplayPage(serialPort), 
      _config(nullptr),
      _commandMgrLink(commandMgrLink),
      _commandMgrComputer(commandMgrComputer),
      _lastHeartbeatSent(0),
      _lastHeartbeatReceived(0),
      _isConnected(false),
      _heartbeatEnabled(true)
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

void BaseBoatPage::setHeartbeatEnabled(bool enabled)
{
    _heartbeatEnabled = enabled;
}

void BaseBoatPage::configUpdated()
{
    // Default implementation - override in derived classes if needed
}

bool BaseBoatPage::isWarning()
{
    return _isWarning;
}

void BaseBoatPage::setWarning(bool warning)
{
    _isWarning = warning;
}

void BaseBoatPage::onConnectionStateChanged(bool connected)
{
    // Default implementation - override in derived classes if needed
    (void)connected;
}

void BaseBoatPage::updateHeartbeat(unsigned long now)
{
    if (!_heartbeatEnabled || !_commandMgrLink)
    {
        return;
    }

    // Check if it's time to send a heartbeat
    if (now - _lastHeartbeatSent >= HEARTBEAT_INTERVAL)
    {
        sendHeartbeat();
        _lastHeartbeatSent = now;
    }

    // Check for connection timeout
    // Only check timeout if we've sent at least one heartbeat
    if (_lastHeartbeatSent > 0)
    {
        bool wasConnected = _isConnected;
        
        // Consider connected if we received a response within timeout period
        _isConnected = (now - _lastHeartbeatReceived) < HEARTBEAT_TIMEOUT;

        // Notify if connection state changed
        if (wasConnected != _isConnected)
        {
			setWarning(!_isConnected);
            onConnectionStateChanged(_isConnected);
        }
    }
}

void BaseBoatPage::sendHeartbeat()
{
    if (_commandMgrLink)
    {
        _commandMgrLink->sendCommand("F0", "");
    }
}

void BaseBoatPage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    if (updateType == static_cast<uint8_t>(PageUpdateType::HeartbeatAck))
    {
        bool wasConnected = _isConnected;
        _lastHeartbeatReceived = millis();
        _isConnected = true;

        // Notify if this is a new connection (first ack or reconnection)
        if (!wasConnected)
        {
            onConnectionStateChanged(true);
        }
    }
    
    BaseDisplayPage::handleExternalUpdate(updateType, data);
}
