#include "ConfigManager.h"
#include <EEPROM.h>

// Static storage (use the shared Config from Config.h)
Config ConfigManager::_cfg;

void ConfigManager::begin()
{
    // On ESP platforms, EEPROM needs begin(size)
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(sizeof(Config));
#endif
}

uint16_t ConfigManager::calcChecksum(const Config& c)
{
    // simple 16-bit sum over bytes excluding checksum field
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&c);
    size_t bytes = sizeof(Config) - sizeof(c.checksum);
    uint32_t sum = 0;
    for (size_t i = 0; i < bytes; ++i) sum += p[i];
    return static_cast<uint16_t>(sum & 0xFFFF);
}

bool ConfigManager::load()
{
    // Ensure EEPROM is initialised for boards that require it
    begin();

    // Read raw bytes
    EEPROM.get(0, _cfg);

    // Quick checks
    if (_cfg.version != VERSION)
    {
        resetToDefaults();
        return false;
    }

    uint16_t expected = calcChecksum(_cfg);
    if (expected != _cfg.checksum)
    {
        // corrupted: reset to defaults
        resetToDefaults();
        return false;
    }

    return true;
}

bool ConfigManager::save()
{
    // prepare checksum
    _cfg.version = VERSION;
    _cfg.checksum = 0;
    _cfg.checksum = calcChecksum(_cfg);

    EEPROM.put(0, _cfg);
#if defined(ESP8266) || defined(ESP32)
    // commit for ESP
    bool ok = EEPROM.commit();
    return ok;
#else
    // AVR writes immediately; assume OK
    return true;
#endif
}

void ConfigManager::resetToDefaults()
{
    // Erase to safe defaults
    memset(&_cfg, 0x00, sizeof(_cfg));
    _cfg.version = VERSION;

    // Default boat name
    strncpy(_cfg.boatName, "My Boat", BOAT_NAME_MAX_LEN);

    // Default relay names (both short and long)
    for (uint8_t i = 0; i < RELAY_COUNT; ++i)
    {
        // Default short name: "R0", "R1", etc.
        char shortBuf[RELAY_SHORT_NAME_MAX_LEN]{ 0 };
        snprintf(shortBuf, sizeof(shortBuf), "R%u", (unsigned)i);
        strncpy(_cfg.relayShortNames[i], shortBuf, RELAY_SHORT_NAME_MAX_LEN - 1);
        _cfg.relayShortNames[i][RELAY_SHORT_NAME_MAX_LEN - 1] = '\0';

        // Default long name: "Relay 0", "Relay 1", etc.
        char longBuf[RELAY_LONG_NAME_MAX_LEN]{ 0 };
        snprintf(longBuf, sizeof(longBuf), "Relay %u", (unsigned)i);
        strncpy(_cfg.relayLongNames[i], longBuf, RELAY_LONG_NAME_MAX_LEN - 1);
        _cfg.relayLongNames[i][RELAY_LONG_NAME_MAX_LEN - 1] = '\0';
    }

    // Default home page mapping: first four relays visible in order
    for (uint8_t i = 0; i < HOME_BUTTONS; ++i)
    {
        _cfg.homePageMapping[i] = i; // map slot i -> relay i
		_cfg.homePageButtonImage[i] = IMG_BTN_COLOR_GREY; // default color
    }

    // compute checksum
    _cfg.checksum = 0;
    _cfg.checksum = calcChecksum(_cfg);
}

Config* ConfigManager::getPtr()
{
    return &_cfg;
}

size_t ConfigManager::availableEEPROMBytes()
{
    return EEPROM.length();
}