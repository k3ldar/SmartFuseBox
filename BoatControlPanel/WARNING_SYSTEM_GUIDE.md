# Warning System with Heartbeat

## Overview

A simple, single-class warning management system with built-in heartbeat monitoring. The `WarningManager` class handles all warnings including connection monitoring through a bitmap-based tracking system.

## Architecture

### WarningManager Class

Single class that manages:
- **Warning State Tracking**: Bitmap tracks up to 32 different warning types
- **Heartbeat Monitoring**: Built-in connection monitoring with automatic F0 command transmission
- **Extensible Warning Types**: Add new warnings by extending the `WarningType` enum

## WarningType Enum

```cpp
enum class WarningType : uint8_t {
    None = 0x00,                    // No warning
    ConnectionLost = 0x01,          // Link heartbeat lost
    HighTemperature = 0x02,         // Temperature threshold exceeded
    LowBattery = 0x03,              // Battery voltage low
    SensorFailure = 0x04,           // Sensor communication failure
    
    // Add new warning types here (0x05 - 0xFF)
};
```

**Adding New Warnings**: Simply add a new enum value and use it anywhere in your code:

```cpp
enum class WarningType : uint8_t {
    // ...existing...
    LowFuel = 0x05,              // New warning type
    GPSFailure = 0x06,           // Another new type
};
```

## Usage

### In Main Code (BoatControlPanel.ino)

```cpp
#include "WarningManager.h"

// Create warning manager with command manager for heartbeat
WarningManager warningManager(&commandMgrLink);

// Create pages with warning manager
HomePage homePage(&NEXTION_SERIAL, &warningManager, &commandMgrLink, &commandMgrComputer);
```

### In Pages (HomePage, WarningPage, etc.)

```cpp
void HomePage::refresh() {
    // Update your UI
    updateTemperature();
    updateHumidity();
    // ...
    
    // Update warnings and heartbeat
    updateWarnings(millis());
    
    // Check warning state and update display
    WarningManager* warningMgr = getWarningManager();
    if (warningMgr && warningMgr->hasWarnings()) {
        setPicture("warning", IMG_WARNING);
    } else {
        setPicture("warning", IMG_BLANK);
    }
    
    // Check connection state
    if (warningMgr && !warningMgr->isConnected()) {
        sendText("humidity", "--");
        sendText("temperature", "--");
    }
}
```

### Handling Heartbeat Acknowledgements

In `AckCommandHandler.cpp`:

```cpp
if (key == "F0" && val.equalsIgnoreCase("ok"))
{
    if (_nextionControl)
    {
        BaseDisplayPage* currentPage = _nextionControl->getCurrentPage();
        if (currentPage)
        {
            currentPage->handleExternalUpdate(
                static_cast<uint8_t>(PageUpdateType::HeartbeatAck), 
                nullptr
            );
        }
    }
}
```

### Raising Warnings from Anywhere

```cpp
// Example: Temperature monitoring
void checkTemperature() {
    float temp = compass.getTemperature();
    
    if (temp > 35.0) {
        warningManager.raiseWarning(WarningType::HighTemperature);
    } else {
        warningManager.clearWarning(WarningType::HighTemperature);
    }
}

// Example: Battery monitoring
void checkBattery() {
    float voltage = readBatteryVoltage();
    
    if (voltage < 11.5) {
        warningManager.raiseWarning(WarningType::LowBattery);
    } else {
        warningManager.clearWarning(WarningType::LowBattery);
    }
}
```

## WarningManager API

### Core Methods

```cpp
// Update heartbeat and check for timeouts
void update(unsigned long now);

// Notify that heartbeat ACK received
void notifyHeartbeatAck();

// Raise/clear warnings
void raiseWarning(WarningType type);
void clearWarning(WarningType type);
void clearAllWarnings();

// Query warnings
bool hasWarnings() const;
bool isWarningActive(WarningType type) const;
bool isConnected() const;

// Configuration
void setHeartbeatEnabled(bool enabled);
void setHeartbeatInterval(unsigned long intervalMs);  // Default: 1000ms
void setHeartbeatTimeout(unsigned long timeoutMs);    // Default: 3000ms
```

## BaseBoatPage Changes

The base class has been simplified:

```cpp
// OLD:
BaseBoatPage(Stream* serialPort, 
             SerialCommandManager* commandMgrLink = nullptr,
             SerialCommandManager* commandMgrComputer = nullptr);

void updateHeartbeat(unsigned long now);
bool isConnected();

// NEW:
BaseBoatPage(Stream* serialPort, 
             WarningManager* warningMgr,
             SerialCommandManager* commandMgrLink = nullptr,
             SerialCommandManager* commandMgrComputer = nullptr);

void updateWarnings(unsigned long now);  // Handles warnings + heartbeat
bool hasWarnings() const;
bool isConnected() const;
WarningManager* getWarningManager() const;
```

## Flow Diagram

```
1. Page refresh() calls updateWarnings(millis())
   ?
   ??> WarningManager::update()
       ??> Send F0 heartbeat every 1000ms
       ??> Check for timeout (3000ms)
           ??> If timeout: raiseWarning(ConnectionLost)

2. When ACK:F0=ok received:
   ?
   AckCommandHandler
   ?
   currentPage->handleExternalUpdate(HeartbeatAck, nullptr)
   ?
   BaseBoatPage::handleExternalUpdate()
   ?
   WarningManager::notifyHeartbeatAck()
   ??> clearWarning(ConnectionLost)

3. Page refresh() checks warnings:
   ?
   if (warningMgr->hasWarnings())
       Show warning icon
   if (!warningMgr->isConnected())
       Show disconnected state
```

## Benefits

1. **Simple**: Single class handles everything
2. **Extensible**: Just add enum values for new warnings
3. **Efficient**: Bitmap-based tracking (up to 32 warnings with 4 bytes)
4. **Independent**: Warnings can be raised from anywhere in code
5. **Clean**: No callbacks or complex state management needed
6. **Maintainable**: All warning logic in one place

## Example: Adding a New Warning

### Step 1: Add to enum (WarningManager.h)

```cpp
enum class WarningType : uint8_t {
    // ...existing...
    LowFuel = 0x05,  // New warning
};
```

### Step 2: Raise warning when needed

```cpp
// Anywhere in your code:
if (fuelLevel < 10.0) {
    warningManager.raiseWarning(WarningType::LowFuel);
} else {
    warningManager.clearWarning(WarningType::LowFuel);
}
```

### Step 3: Handle in UI (optional)

```cpp
void HomePage::refresh() {
    updateWarnings(millis());
    
    WarningManager* warningMgr = getWarningManager();
    if (warningMgr) {
        // Check specific warning
        if (warningMgr->isWarningActive(WarningType::LowFuel)) {
            sendText("fuelWarning", "LOW FUEL!");
        }
        
        // Or just show general warning indicator
        if (warningMgr->hasWarnings()) {
            setPicture("warning", IMG_WARNING);
        }
    }
}
```

That's it! No complex setup, no callbacks, no multiple classes.

## Configuration

### Heartbeat Timing

```cpp
// In setup() or wherever appropriate:
warningManager.setHeartbeatInterval(1000);  // Send every 1 second
warningManager.setHeartbeatTimeout(3000);   // Timeout after 3 seconds

// Disable if needed:
warningManager.setHeartbeatEnabled(false);
```

### Checking Multiple Warnings

```cpp
WarningManager* wm = getWarningManager();
if (wm) {
    if (wm->isWarningActive(WarningType::ConnectionLost)) {
        // Handle connection loss
    }
    if (wm->isWarningActive(WarningType::HighTemperature)) {
        // Handle high temperature
    }
    if (wm->isWarningActive(WarningType::LowBattery)) {
        // Handle low battery
    }
}
```

## Migration from Old System

### Changes Required:

1. **BaseBoatPage constructor**: Add `WarningManager*` parameter
2. **`updateHeartbeat()` ? `updateWarnings()`**: Rename method call
3. **`onConnectionStateChanged()` ? removed**: Update display in `refresh()` instead
4. **Create `WarningManager` instance**: In main .ino file

### Before:

```cpp
HomePage homePage(&NEXTION_SERIAL, &commandMgrLink, &commandMgrComputer);

void HomePage::refresh() {
    updateHeartbeat(millis());
}

void HomePage::onConnectionStateChanged(bool connected) {
    if (connected) {
        setPicture("warning", IMG_BLANK);
    } else {
        setPicture("warning", IMG_WARNING);
    }
}
```

### After:

```cpp
WarningManager warningManager(&commandMgrLink);
HomePage homePage(&NEXTION_SERIAL, &warningManager, &commandMgrLink, &commandMgrComputer);

void HomePage::refresh() {
    updateWarnings(millis());
    
    WarningManager* wm = getWarningManager();
    if (wm && wm->hasWarnings()) {
        setPicture("warning", IMG_WARNING);
    } else {
        setPicture("warning", IMG_BLANK);
    }
}
```
