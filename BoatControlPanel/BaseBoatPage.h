#pragma once

#include <Arduino.h>
#include <BaseDisplayPage.h>
#include <SerialCommandManager.h>
#include "Config.h"

const uint8_t PAGE_HOME = 0;
const uint8_t PAGE_WARNING = 1;
const uint8_t PAGE_BUTTONS = 2;

const uint8_t ImageWarning = 10;
const uint8_t ImageBlank = 11;


// Update type constants for HomePage external updates
enum class PageUpdateType : uint8_t {
    None = 0x00,
	Warning = 0x01,
    RelayState = 0x02,
    HeartbeatAck = 0x03,  // Heartbeat acknowledgement received (F0=ok)
};

// Data structure for relay state updates
struct RelayStateUpdate {
    uint8_t relayIndex;  // 0-based relay index (0..7)
    bool isOn;           // true = relay on, false = relay off
};

/**
 * @class BaseBoatPage
 * @brief Intermediate base class for all boat control panel pages.
 * 
 * Provides common functionality and shared state for all pages in the boat
 * control panel application, including warning state management, configuration
 * access, heartbeat/connection monitoring, and other shared behaviors.
 */
class BaseBoatPage : public BaseDisplayPage {
private:
    // Shared configuration pointer
    Config* _config;

    // Command managers shared across all pages
    SerialCommandManager* _commandMgrLink;
    SerialCommandManager* _commandMgrComputer;

    // Heartbeat and connection monitoring
    unsigned long _lastHeartbeatSent;
    unsigned long _lastHeartbeatReceived;
    bool _isConnected;
    bool _heartbeatEnabled;

    /**
     * @brief Internal method to send heartbeat command.
     * Called automatically from updateHeartbeat().
     */
    void sendHeartbeat();

protected:
    // Static warning flag shared across all pages
    static bool _isWarning;

    // Heartbeat configuration constants (can be overridden by derived classes if needed)
    static const unsigned long HEARTBEAT_INTERVAL = 1000;  // Send heartbeat every 1 second
    static const unsigned long HEARTBEAT_TIMEOUT = 3000;   // Connection lost after 3 seconds without response
    
    /**
     * @brief Constructor for boat pages.
     * @param serialPort Pointer to the Nextion serial stream
     * @param commandMgrLink Pointer to the SerialCommandManager for link communication (optional)
     * @param commandMgrComputer Pointer to the SerialCommandManager for computer communication (optional)
     */
    explicit BaseBoatPage(Stream* serialPort, 
                         SerialCommandManager* commandMgrLink = nullptr,
                         SerialCommandManager* commandMgrComputer = nullptr);
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~BaseBoatPage();
    
    /**
     * @brief Set the system warning state.
     * @param warning true to activate warning, false to clear
     */
    static void setWarning(bool warning);
    
    /**
     * @brief Get the configuration pointer.
     * @return Pointer to the configuration, or nullptr if not set
     */
    Config* getConfig() const { return _config; }

    /**
     * @brief Get the link command manager.
     * @return Pointer to the link SerialCommandManager, or nullptr if not set
     */
    SerialCommandManager* getCommandMgrLink() const { return _commandMgrLink; }

    /**
     * @brief Get the computer command manager.
     * @return Pointer to the computer SerialCommandManager, or nullptr if not set
     */
    SerialCommandManager* getCommandMgrComputer() const { return _commandMgrComputer; }

    /**
     * @brief Called when connection state changes.
     * 
     * Override in derived classes to respond to connection state changes.
     * Default implementation does nothing.
     * 
     * @param connected true if connection established, false if connection lost
     */
    virtual void onConnectionStateChanged(bool connected);

    /**
     * @brief Update heartbeat mechanism.
     * 
     * Call this from your derived class's refresh() method to maintain heartbeat.
     * Handles sending heartbeat commands at intervals and detecting timeouts.
     * 
     * @param now Current time in milliseconds (from millis())
     */
    void updateHeartbeat(unsigned long now);

    /**
     * @brief Handle external updates including heartbeat acknowledgements.
     * 
     * Override in derived classes to handle page-specific updates.
     * Base implementation handles HeartbeatAck update type.
     * Call base implementation first when overriding.
     * 
     * @param updateType Type of update (see PageUpdateType enum)
     * @param data Optional data pointer (type depends on updateType)
     */
    void handleExternalUpdate(uint8_t updateType, const void* data) override;

public:
    
    /**
     * @brief Set the configuration pointer for this page.
     * @param config Pointer to the global configuration
     */
    void configSet(Config* config);

    /**
     * @brief Enable or disable heartbeat functionality.
     * 
     * @param enabled true to enable heartbeat, false to disable
     */
    void setHeartbeatEnabled(bool enabled);

    /**
     * @brief Called when configuration has been updated.
     * 
     * Override in derived classes to respond to configuration changes.
     * Default implementation does nothing. This is public so external
     * command handlers can notify pages when config changes.
     */
    virtual void configUpdated();

    /**
     * @brief Check if system is in warning state.
     * @return true if warning is active, false otherwise
     */
    static bool isWarning();

    /**
     * @brief Check if link connection is active.
     * @return true if connected (heartbeat acknowledged within timeout), false otherwise
     */
    bool isConnected() const { return _isConnected; }
};
