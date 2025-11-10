#pragma once

#include <Arduino.h>
#include "BaseCommandHandler.h"
#include "NextionControl.h"
#include "WarningManager.h"

/**
 * @brief Base class for command handlers that interact with boat-specific systems.
 * 
 * This class extends BaseCommandHandler with common dependencies and helper methods
 * used by command handlers that need to:
 * - Send debug messages to the computer command manager
 * - Notify the current Nextion display page of updates
 * - Access the warning management system
 * - Parse common data formats (booleans, digits)
 * 
 * Handlers like AckCommandHandler and SensorCommandHandler should inherit from this
 * class to get access to these shared capabilities without code duplication.
 * 
 * For handlers that don't need these boat-specific dependencies (like ConfigCommandHandler),
 * inherit directly from BaseCommandHandler instead.
 */
class BaseBoatCommandHandler : public BaseCommandHandler
{
protected:
    /**
     * @brief Constructor with boat-specific dependencies.
     * 
     * @param computerCommandManager Manager for sending debug/error messages to computer
     * @param nextionControl Controller for interacting with Nextion display
     * @param warningManager Manager for system warnings (can be nullptr if not needed)
     */
    BaseBoatCommandHandler(
        SerialCommandManager* computerCommandManager,
        NextionControl* nextionControl,
        WarningManager* warningManager = nullptr
    );

    /**
     * @brief Notify the current display page of an external update.
     * 
     * This is a convenience wrapper that safely gets the current page from
     * NextionControl and calls handleExternalUpdate on it.
     * 
     * @param updateType Type of update (cast to uint8_t from PageUpdateType enum)
     * @param data Optional pointer to update-specific data structure
     */
    void notifyCurrentPage(uint8_t updateType, const void* data);

    /**
     * @brief Send a debug message to the computer command manager.
     * 
     * Automatically includes the handler's identifier in the message.
     * Override getHandlerIdentifier() to customize the identifier string.
     * 
     * @param message Debug message content
	 * @param identifier Command handler identifier
     */
    void sendDebugMessage(const String& message, const String& identifier);

    /**
     * @brief Parse a string value as a boolean.
     * 
     * Accepts multiple formats:
     * - "1" or "0"
     * - "on" or "off" (case-insensitive)
     * - "true" or "false" (case-insensitive)
     * 
     * @param value String to parse
     * @return true if the value represents a truthy value, false otherwise
     */
    bool parseBooleanValue(const String& value) const;

    /**
     * @brief Check if a string contains only digits.
     * 
     * @param s String to check
     * @return true if the string is non-empty and contains only digits (0-9)
     */
    bool isAllDigits(const String& s) const;

    // Protected member variables for derived classes to access
    SerialCommandManager* _computerCommandManager;
    NextionControl* _nextionControl;
    WarningManager* _warningManager;
};
