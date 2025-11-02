#pragma once
#include <Arduino.h>
#include <stdint.h>



// Button color constants (Nextion picture IDs)
const uint8_t IMG_BTN_COLOR_DEFAULT = 0xFF; // Use default color
const uint8_t IMG_BTN_COLOR_BLUE = 2;
const uint8_t IMG_BTN_COLOR_GREEN = 3;
const uint8_t IMG_BTN_COLOR_GREY = 4;
const uint8_t IMG_BTN_COLOR_ORANGE = 5;
const uint8_t IMG_BTN_COLOR_RED = 6;
const uint8_t IMG_BTN_COLOR_YELLOW = 7;

const uint8_t IMG_COMPASS = 8;
const uint8_t IMG_SETTINGS = 9;
const uint8_t IMG_WARNING = 10;
const uint8_t IMG_BLANK = 11;
const uint8_t IMG_BACK_BUTTON = 12;


//
// Layout:
// - version (uint8_t)
// - boatName (30 chars + null)
// - relayNames[8][6] (5 chars + null each)
// - homePageMapping[4] (values 0..7 or 0xFF for empty)
// - enabledRelaysMask (uint8_t bitmask for 8 relays)
// - checksum (uint16_t)
//
// Keep struct packed and stable. Increase 'VERSION' when you change layout.
// Packed POD for persistent configuration.
const uint8_t VERSION = 0;
const uint8_t RELAY_COUNT = 8;
const uint8_t HOME_BUTTONS = 4;
const uint8_t BOAT_NAME_MAX_LEN = 31; // max characters (inc null)
const uint8_t RELAY_NAME_MAX_LEN = 6; // max characters (inc null)

struct Config {
    uint8_t version;
    char boatName[BOAT_NAME_MAX_LEN];
    char relayNames[RELAY_COUNT][RELAY_NAME_MAX_LEN];
    uint8_t homePageMapping[HOME_BUTTONS]; // 0..3 or 0xFF = empty
    uint8_t homePageButtonImage[HOME_BUTTONS]; //0..3 or 0xFF = empty
    uint16_t checksum;
} __attribute__((packed));