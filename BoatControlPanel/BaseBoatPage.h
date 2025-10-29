#pragma once

#include <Arduino.h>
#include <BaseDisplayPage.h>
#include "Config.h"


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
 * access, and other shared behaviors.
 */
class BaseBoatPage : public BaseDisplayPage {
private:
    // Shared configuration pointer
    Config* _config;

protected:
    // Static warning flag shared across all pages
    static bool _isWarning;
    
    /**
     * @brief Constructor for boat pages.
     * @param serialPort Pointer to the Nextion serial stream
     */
    explicit BaseBoatPage(Stream* serialPort);
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~BaseBoatPage();
    
    /**
     * @brief Check if system is in warning state.
     * @return true if warning is active, false otherwise
     */
    static bool isWarning();
    
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
