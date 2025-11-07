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
const uint8_t IMG_BTN_LARGE_OFFSET = 12; // Offset for large button images

const uint8_t IMG_COMPASS = 8;
const uint8_t IMG_SETTINGS = 9;
const uint8_t IMG_WARNING = 10;
const uint8_t IMG_BLANK = 11;
const uint8_t IMG_BACK_BUTTON = 12;


//
// Layout:
// - version (uint8_t)
// - boatName (30 chars + null)
// - relayShortNames[8][6] (5 chars + null each)
// - relayLongNames[8][21] (20 chars + null each)
// - homePageMapping[4] (values 0..7 or 0xFF for empty)
// - homePageButtonImage[4] (button color image IDs)
// - checksum (uint16_t)
//
// Keep struct packed and stable. Increase 'VERSION' when you change layout.
// Packed POD for persistent configuration.
const uint8_t VERSION = 0;
const uint8_t RELAY_COUNT = 8;
const uint8_t HOME_BUTTONS = 4;
const uint8_t BOAT_NAME_MAX_LEN = 31; // max characters (inc null)
const uint8_t RELAY_SHORT_NAME_MAX_LEN = 6; // max characters (inc null) - for home page
const uint8_t RELAY_LONG_NAME_MAX_LEN = 21; // max characters (inc null) - for buttons page

struct Config {
    uint8_t version;
    char boatName[BOAT_NAME_MAX_LEN];
    char relayShortNames[RELAY_COUNT][RELAY_SHORT_NAME_MAX_LEN];
    char relayLongNames[RELAY_COUNT][RELAY_LONG_NAME_MAX_LEN];
    uint8_t homePageMapping[HOME_BUTTONS]; // 0..7 or 0xFF = empty
    uint8_t homePageButtonImage[HOME_BUTTONS]; // 0..7 or 0xFF = empty
    uint16_t checksum;
} __attribute__((packed));