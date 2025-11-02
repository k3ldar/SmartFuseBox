#pragma once

#include <Arduino.h>
#include <BaseDisplayPage.h>
#include <SerialCommandManager.h>
#include "Config.h"
#include "WarningManager.h"

const uint8_t PAGE_HOME = 0;
const uint8_t PAGE_WARNING = 1;
const uint8_t PAGE_BUTTONS = 2;

const uint8_t ImageWarning = 10;
const uint8_t ImageBlank = 11;

// Update type constants for external updates
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
 * access, and other shared behaviors.
 */
class BaseBoatPage : public BaseDisplayPage {
private:
    // Shared configuration pointer
    Config* _config;

    // Command managers shared across all pages
    SerialCommandManager* _commandMgrLink;
    SerialCommandManager* _commandMgrComputer;

    // Warning manager (shared across all pages)
    WarningManager* _warningManager;

protected:
    
    /**
     * @brief Constructor for boat pages.
     * @param serialPort Pointer to the Nextion serial stream
     * @param warningMgr Pointer to the shared WarningManager
     * @param commandMgrLink Pointer to the SerialCommandManager for link communication (optional)
     * @param commandMgrComputer Pointer to the SerialCommandManager for computer communication (optional)
     */
    explicit BaseBoatPage(Stream* serialPort, 
                         WarningManager* warningMgr,
                         SerialCommandManager* commandMgrLink = nullptr,
                         SerialCommandManager* commandMgrComputer = nullptr);
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~BaseBoatPage();
    
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
     * @brief Get the warning manager.
     * @return Pointer to the WarningManager
     */
    WarningManager* getWarningManager() const { return _warningManager; }

public:
    
    /**
     * @brief Set the configuration pointer for this page.
     * @param config Pointer to the global configuration
     */
    void configSet(Config* config);

    /**
     * @brief Called when configuration has been updated.
     * 
     * Override in derived classes to respond to configuration changes.
     * Default implementation does nothing. This is public so external
     * command handlers can notify pages when config changes.
     */
    virtual void configUpdated();
};
