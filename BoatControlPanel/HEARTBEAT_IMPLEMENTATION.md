# Heartbeat Implementation Summary

## Overview
Implemented a robust heartbeat mechanism in the Boat Control Panel to monitor the connection between the control panel and the fuse box. The heartbeat sends `F0` commands every second and monitors for `ACK:F0=ok` responses.

## Architecture

### BaseBoatPage (Shared Functionality)
The heartbeat functionality is implemented in `BaseBoatPage` to be shared across all pages. **The `SerialCommandManager` pointers for both link and computer communication are stored in the base class and passed via constructor**, eliminating redundancy across derived pages.

**Features:**
- **Constructor-based initialization** - Command managers passed during construction, not via setters
- **Unified update mechanism** - Uses existing `handleExternalUpdate()` for heartbeat ACKs
- **Centralized command managers** - Both `_commandMgrLink` and `_commandMgrComputer` stored in base class
- **Automatic heartbeat sending** - Sends `F0` command every 1000ms (configurable via `HEARTBEAT_INTERVAL`)
- **Timeout detection** - Detects connection loss if no response within 3000ms (configurable via `HEARTBEAT_TIMEOUT`)
- **Connection state tracking** - Maintains `_isConnected` flag indicating connection status
- **Virtual callbacks** - `onConnectionStateChanged(bool connected)` called when connection state changes
- **Non-blocking** - Uses timer-based mechanism, no delays

**Key Methods:**
- **Constructor** - `BaseBoatPage(Stream*, SerialCommandManager* link = nullptr, SerialCommandManager* computer = nullptr)`
- `getCommandMgrLink()` - Get the link command manager pointer
- `getCommandMgrComputer()` - Get the computer command manager pointer
- `setHeartbeatEnabled(bool)` - Enable/disable heartbeat functionality
- `updateHeartbeat(unsigned long now)` - Call from `refresh()` to maintain heartbeat
- `handleExternalUpdate(uint8_t, const void*)` - Override to handle page-specific updates (call base first!)
- `isConnected()` - Query current connection status
- `onConnectionStateChanged(bool)` - Override to handle state changes

### AckCommandHandler
Updated to detect `ACK:F0=ok` and notify the active page using the standard `handleExternalUpdate` mechanism:

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

No special casting or page-specific methods needed - uses the polymorphic `handleExternalUpdate` interface.

### HomePage Implementation
Example implementation showing clean constructor-based initialization:

**Constructor with Command Managers:**
```cpp
HomePage::HomePage(Stream* serialPort,
                   SerialCommandManager* commandMgrLink,
                   SerialCommandManager* commandMgrComputer)
    : BaseBoatPage(serialPort, commandMgrLink, commandMgrComputer)
{
}
```

**Accessing Command Managers:**
```cpp
void HomePage::handleTouch(uint8_t compId, uint8_t eventType)
{
    // Get command managers from base class
    SerialCommandManager* commandMgrLink = getCommandMgrLink();
    SerialCommandManager* commandMgrComputer = getCommandMgrComputer();
    
    if (commandMgrLink)
    {
        commandMgrLink->sendCommand("R3", cmd);
    }
    
    if (commandMgrComputer)
    {
        commandMgrComputer->sendDebug("Button pressed", "HomePage");
    }
}
```

**Override handleExternalUpdate (IMPORTANT - Call base first!):**
```cpp
void HomePage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    // ALWAYS call base class first to handle heartbeat ACKs
    BaseBoatPage::handleExternalUpdate(updateType, data);

    // Then handle page-specific updates
    if (updateType == static_cast<uint8_t>(PageUpdateType::RelayState) && data != nullptr)
    {
        const RelayStateUpdate* update = static_cast<const RelayStateUpdate*>(data);
        // Handle relay state update...
    }
}
```

**In `refresh()`:**
```cpp
void HomePage::refresh()
{
    // ...existing code...
    
    // Update heartbeat mechanism
    updateHeartbeat(millis());
}
```

**Connection state callback:**
```cpp
void HomePage::onConnectionStateChanged(bool connected)
{
    SerialCommandManager* commandMgrComputer = getCommandMgrComputer();
    
    if (connected)
    {
        if (commandMgrComputer)
        {
            commandMgrComputer->sendDebug("Link connection established", "HomePage");
        }
        // Update UI: setPicture("connectionIndicator", IMG_CONNECTED);
    }
    else
    {
        if (commandMgrComputer)
        {
            commandMgrComputer->sendDebug("Link connection lost", "HomePage");
        }
        // Update UI: setPicture("connectionIndicator", IMG_DISCONNECTED);
    }
}
```

## Configuration

### Timing Constants (in BaseBoatPage.h)
```cpp
static const unsigned long HEARTBEAT_INTERVAL = 1000;  // Send every 1 second
static const unsigned long HEARTBEAT_TIMEOUT = 3000;   // Timeout after 3 seconds
```

These can be overridden in derived classes if needed.

## Setup Requirements

In `BoatControlPanel.ino`:

**Clean Constructor-Based Initialization:**
```cpp
// Command managers passed directly to constructor
HomePage homePage(&NEXTION_SERIAL, &commandMgrLink, &commandMgrComputer);
BaseDisplayPage* pages[] = { &homePage };
NextionControl nextion(&NEXTION_SERIAL, pages, sizeof(pages) / sizeof(pages[0]));
```

No setter methods needed - everything configured at construction time!

### Benefits of This Design:

? **Constructor injection** - Command managers set during object creation, ensuring they're always available  
? **No setter methods** - Cleaner API, fewer steps to initialize  
? **Unified update mechanism** - Uses existing `handleExternalUpdate()` polymorphic interface  
? **No special methods** - No `notifyHeartbeatAck()` or setter methods needed
? **Polymorphic** - Works through base class interface
? **Automatic** - No manual management needed, just call `updateHeartbeat()` in `refresh()`
? **Non-blocking** - Uses timer-based approach, no delays
? **Reusable** - Implemented in base class, available to all pages
? **Flexible** - Virtual callbacks allow custom responses per page
? **Configurable** - Timing constants can be adjusted
? **Reliable** - Proper state management with transitions
? **C++14 Compatible** - No modern C++ features required
? **Clean design** - Follows SOLID principles

## How It Works

### Flow Diagram
```
1. NextionControl calls refresh() every 1000ms
   ?
   ??> HomePage::refresh()
   ?    ??> updateHeartbeat(millis())
   ?    ?    ??> Check if 1000ms elapsed since last send
   ?    ?    ?   ??> Send F0 command via _commandMgrLink
   ?    ?    ?
   ?    ?    ??> Check if 3000ms elapsed since last ACK
   ?    ?         ??> If timeout: set _isConnected = false
   ?    ?              ??> Call onConnectionStateChanged(false)
   ?
2. Fuse box receives F0 command
   ??> Responds with ACK:F0=ok
       ?
3. SerialCommandManager receives ACK:F0=ok
   ??> Calls AckCommandHandler::handleCommand()
       ??> Detects F0=ok parameter
           ??> Calls currentPage->handleExternalUpdate(PageUpdateType::HeartbeatAck, nullptr)
               ?
4. HomePage::handleExternalUpdate() called
   ??> Calls BaseBoatPage::handleExternalUpdate() FIRST
       ??> BaseBoatPage handles HeartbeatAck:
           ??> Updates _lastHeartbeatReceived timestamp
           ??> Sets _isConnected = true
           ??> If was disconnected: Call onConnectionStateChanged(true)
                ??> HomePage::onConnectionStateChanged(true) called
```

## Extending to Other Pages

Any page that extends `BaseBoatPage` can use heartbeat:

```cpp
class SettingsPage : public BaseBoatPage
{
public:
    // Pass command managers via constructor
    SettingsPage(Stream* serialPort, 
                 SerialCommandManager* commandMgrLink,
                 SerialCommandManager* commandMgrComputer)
        : BaseBoatPage(serialPort, commandMgrLink, commandMgrComputer)
    {
    }

protected:
    void refresh() override
    {
        // Update your UI
        // ...
        
        // Enable heartbeat monitoring
        updateHeartbeat(millis());
    }
    
    // IMPORTANT: Call base class first!
    void handleExternalUpdate(uint8_t updateType, const void* data) override
    {
        BaseBoatPage::handleExternalUpdate(updateType, data);
        
        // Handle page-specific updates...
    }
    
    void onConnectionStateChanged(bool connected) override
    {
        // React to connection changes
        if (!connected)
        {
            sendText("warning", "Connection Lost!");
        }
    }
};
```

## Testing

### Manual Testing
1. Power on both control panel and fuse box
2. Observe debug messages: "Heartbeat ACK received" every second
3. Power off fuse box
4. After 3 seconds, observe: "Link connection lost"
5. Power on fuse box
6. Observe: "Link connection established"

### Debug Output
Enable debug output to see heartbeat activity:
- `Heartbeat ACK received` - Every time ACK:F0=ok is received
- `Link connection established` - When connection is restored
- `Link connection lost` - When no ACK for 3 seconds

## Benefits Summary

1. **Constructor injection** - Dependencies set at construction time
2. **Unified interface** - Uses existing `handleExternalUpdate()` mechanism
3. **No special methods** - No `notifyHeartbeatAck()` or setter methods needed
4. **Polymorphic** - Works through base class interface
5. **Automatic** - No manual management needed, just call `updateHeartbeat()` in `refresh()`
6. **Non-blocking** - Uses timer-based approach, no delays
7. **Reusable** - Implemented in base class, available to all pages
8. **Flexible** - Virtual callbacks allow custom responses per page
9. **Configurable** - Timing constants can be adjusted
10. **Reliable** - Proper state management with transitions
11. **C++14 Compatible** - No modern C++ features required
12. **Clean design** - Follows SOLID principles

## Important Notes

?? **When overriding `handleExternalUpdate` in derived classes, ALWAYS call the base class implementation first!**

```cpp
void YourPage::handleExternalUpdate(uint8_t updateType, const void* data)
{
    // REQUIRED: Call base class first to handle heartbeat ACKs
    BaseBoatPage::handleExternalUpdate(updateType, data);
    
    // Then handle your page-specific updates
    if (updateType == ...) { ... }
}
```

This ensures the base class can handle heartbeat acknowledgements before your derived class processes other update types.
