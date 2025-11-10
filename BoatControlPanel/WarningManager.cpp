#include "WarningManager.h"

WarningManager::WarningManager(SerialCommandManager* commandMgr, unsigned long heartbeatInterval, unsigned long heartbeatTimeout)
    : _commandMgr(commandMgr),
      _activeWarnings(0),
      _heartbeatInterval(heartbeatInterval),
      _heartbeatTimeout(heartbeatTimeout),
      _lastHeartbeatSent(0),
      _lastHeartbeatReceived(0),
      _heartbeatEnabled(heartbeatInterval > 0)
{
}

void WarningManager::update(unsigned long now)
{
    if (_heartbeatEnabled && _commandMgr)
    {
        updateConnection(now);
    }
}

void WarningManager::notifyHeartbeatAck()
{
    _lastHeartbeatReceived = millis();
    
    // Clear the connection lost warning (connection is now established)
    clearWarning(WarningType::ConnectionLost);
}

void WarningManager::raiseWarning(WarningType type)
{
    if (type == WarningType::None)
        return;
    
    uint8_t bit = static_cast<uint8_t>(type);
    if (bit < 32)  // Ensure we don't overflow the bitmap
    {
        _activeWarnings |= (1UL << bit);
    }
}

void WarningManager::clearWarning(WarningType type)
{
    if (type == WarningType::None)
        return;
    
    uint8_t bit = static_cast<uint8_t>(type);
    if (bit < 32)
    {
        _activeWarnings &= ~(1UL << bit);
    }
}

void WarningManager::clearAllWarnings()
{
    _activeWarnings = 0;
}

bool WarningManager::hasWarnings() const
{
    return _activeWarnings != 0;
}

bool WarningManager::isWarningActive(WarningType type) const
{
    if (type == WarningType::None)
        return false;
    
    uint8_t bit = static_cast<uint8_t>(type);
    if (bit < 32)
    {
        return (_activeWarnings & (1UL << bit)) != 0;
    }
    
    return false;
}

void WarningManager::sendHeartbeat()
{
    if (_commandMgr)
    {
        _commandMgr->sendCommand(SystemHeartbeatCommand, "");
    }
}

void WarningManager::updateConnection(unsigned long now)
{
    // Send heartbeat if interval elapsed
    if (now - _lastHeartbeatSent >= _heartbeatInterval)
    {
        sendHeartbeat();
        _lastHeartbeatSent = now;
    }

    // Check for timeout (only after we've sent at least one heartbeat)
    if (_lastHeartbeatSent > 0 || now >= _heartbeatTimeout)
    {
        bool connected = (_lastHeartbeatReceived > 0) && 
                        (now - _lastHeartbeatReceived) < _heartbeatTimeout;

        // Update warning state based on connection status
        if (connected)
        {
            clearWarning(WarningType::ConnectionLost);
        }
        else
        {
            raiseWarning(WarningType::ConnectionLost);
        }
    }
}
