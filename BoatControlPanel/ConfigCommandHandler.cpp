#include "ConfigCommandHandler.h"

ConfigCommandHandler::ConfigCommandHandler(HomePage* homePage)
    : _homePage(homePage)
{
}

bool ConfigCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    // Access the in-memory config
    Config* cfg = ConfigManager::getPtr();
    if (!cfg)
    {
        sendAckErr(sender, command, "Config not available");
        return true;
    }

    // Normalize command
    String cmd = command;
    cmd.trim();

    if (cmd == ConfigRenameBoat)
    {
        if (paramCount >= 1)
        {
            // Expect "C2:name=<value>" where value is the boat name (or just a single token)
            String name = params[0].value;
            if (name.length() == 0)
                name = params[0].key;

            name.trim();
            if (name.length() == 0)
            {
                sendAckErr(sender, cmd, "Empty name", &params[0]);
                return true;
            }

            // enforce max length BOAT_NAME_MAX_LEN (defined in ConfigManager / Config.h)
            strncpy(cfg->boatName, name.c_str(), sizeof(cfg->boatName) - 1);
            cfg->boatName[sizeof(cfg->boatName) - 1] = '\0';
            sendAckOk(sender, cmd, &params[0]);
        }
        else
        {
            sendAckErr(sender, cmd, "Missing param");
        }
    }
    else if (cmd == ConfigRenameRelay)
    {
        // Expect "C3:<idx>=<name>" where idx 0..7
        if (paramCount >= 1)
        {
            int idx = params[0].key.toInt();
            String name = params[0].value;
            if (name.length() == 0)
            {
                // fallback if they sent single token e.g. "RNAME 2" (no name) -> error
                sendAckErr(sender, cmd, "Missing name", &params[0]);
                return true;
            }

            if (idx < 0 || idx >= (int)RELAY_COUNT)
            {
                sendAckErr(sender, cmd, "Index out of range", &params[0]);
                return true;
            }

            // copy with truncation to relay name length
            size_t maxLen = sizeof(cfg->relayNames[idx]) - 1;
            strncpy(cfg->relayNames[idx], name.c_str(), maxLen);
            cfg->relayNames[idx][maxLen] = '\0';

            sendAckOk(sender, cmd, &params[0]);
        }
        else
        {
            sendAckErr(sender, cmd, "Missing params");
        }
    }
    else if (cmd == ConfigMapHomeButton)
    {
        // Expect "MAP <button>=<relay>" where button 0..3, relay 0..7 (or 255 to unmap)
        if (paramCount >= 1)
        {
            int button = params[0].key.toInt();
            int relay = params[0].value.toInt(); // if value empty, toInt() -> 0

            if (button < 0 || button >= (int)HOME_BUTTONS)
            {
                sendAckErr(sender, cmd, "Button out of range", &params[0]);
                return true;
            }

            if ((relay < 0 || relay >= (int)RELAY_COUNT) && relay != IMG_BTN_COLOR_DEFAULT)
            {
                sendAckErr(sender, cmd, "Relay out of range (or 255 to clear)", &params[0]);
                return true;
            }

            cfg->homePageMapping[button] = (uint8_t)relay;
            sendAckOk(sender, cmd, &params[0]);
        }
        else
        {
            sendAckErr(sender, cmd, "Missing params");
        }
    }
    else if (command == ConfigSetHomeButtonColor) 
    {
        // Expect "MAP <button>=<color>" where button 0..3, image 0..5 (or 255 to unmap)
        if (paramCount >= 1)
        {
            int button = params[0].key.toInt();
            int buttonColor = params[0].value.toInt(); 

            if (buttonColor < 0xFF)
			    buttonColor += 2; // Adjust to match BTN_COLOR_* constants (2..7), 255 to clear

            if (button < 0 || button >= (int)HOME_BUTTONS)
            {
                sendAckErr(sender, cmd, "Button out of range", &params[0]);
                return true;
            }
            
            if ((buttonColor < IMG_BTN_COLOR_BLUE || buttonColor > (int)IMG_BTN_COLOR_YELLOW) && buttonColor != IMG_BTN_COLOR_DEFAULT)
            {
                sendAckErr(sender, cmd, "Button out of range (or 255 to clear)", &params[0]);
                return true;
            }

            cfg->homePageButtonImage[button] = (uint8_t)buttonColor;
            sendAckOk(sender, cmd, &params[0]);
        }
        else
        {
            sendAckErr(sender, cmd, "Missing params");
        }
    }
    else if (cmd == ConfigSaveSettings)
    {
        // Recompute checksum and persist to EEPROM
        bool ok = ConfigManager::save();
        if (ok)
        {
            // Preserve previous explicit SAVED token
            sendAckOk(sender, cmd);
        }
        else
        {
            sendAckErr(sender, cmd, "EEPROM commit failed");
        }
    }
    else if (cmd == ConfigGetSettings)
    {
        // return summary of config back to caller in multiple commands
        // C2:<name>
        sender->sendCommand(ConfigRenameBoat, String(cfg->boatName));

        // C3 entries
        for (uint8_t i = 0; i < RELAY_COUNT; ++i)
        {
            sender->sendCommand(ConfigRenameRelay, String(i) + "=" + String(cfg->relayNames[i]));
        }

        // C5 entries
        for (uint8_t s = 0; s < HOME_BUTTONS; ++s)
        {
            sender->sendCommand(ConfigMapHomeButton, String(s) + "=" + String(cfg->homePageMapping[s]));
        }

        // C6 Send home page button color mappings
        for (uint8_t i = 0; i < HOME_BUTTONS; i++)
        {
            String colorMapping = String(i) + "=" + String(cfg->homePageButtonImage[i]);
            sender->sendCommand(ConfigSetHomeButtonColor, colorMapping);
        }

        sendAckOk(sender, cmd);
    }
    else if (cmd == ConfigResetSettings)
    {
        // Reset to defaults
        ConfigManager::resetToDefaults();
        sendAckOk(sender, cmd);
	}
    else
    {
        sendAckErr(sender, cmd, "Unknown config command");
    }

    // Notify UI
    if (_homePage)
        _homePage->configUpdated();
}

const String* ConfigCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { ConfigSaveSettings, ConfigGetSettings, ConfigResetSettings, ConfigRenameBoat,
        ConfigRenameRelay, ConfigMapHomeButton, ConfigSetHomeButtonColor };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}