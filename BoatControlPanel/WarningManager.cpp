#include "WarningManager.h"

WarningManager::WarningManager(SerialCommandManager* commandMgr, unsigned long hearbeatInterval, unsigned long hearbeatTimeout)
    : _commandMgr(commandMgr),
      _activeWarnings(0),
      _heartbeatInterval(hearbeatInterval),
      _heartbeatTimeout(hearbeatTimeout),
      _lastHeartbeatSent(0),
      _lastHeartbeatReceived(0),
      _heartbeatEnabled(hearbeatInterval > 0),
      _isConnected(false)
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
    
    // If we weren't connected, we are now
    if (!_isConnected)
    {
        _isConnected = true;
        clearWarning(WarningType::ConnectionLost);
    }
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
        _commandMgr->sendCommand("F0", "");
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

        // Update state if changed
        if (connected != _isConnected)
        {
            _isConnected = connected;
            
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
}
