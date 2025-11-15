#pragma once

#include <stdint.h>


constexpr uint8_t DefaultValue = 0xFF;


enum class VesselType : uint8_t {
    Motor = 0x00,                   // Power boat
    Sail = 0x01,                    // Sailing boat
    Fishing = 0x02,                 // Fishing boat
    Yacht = 0x03,                   // Yacht
};

//
// Layout:
// - version (uint8_t)
// - vesselType (VesselType)
// - hornRelayIndex (uint8_t) 0..7 or 0xFF = none
// - soundStartDelayMs (uint16_t)
// - checksum (uint16_t)
//
// Keep struct packed and stable. Increase 'VERSION' when you change layout.
// Packed POD for persistent configuration.
constexpr uint8_t ConfigVersion = 3;
constexpr uint8_t ConfigRelayCount = 8;

struct Config {
    uint8_t version;
    VesselType vesselType;
    uint8_t hornRelayIndex; // 0..7 or 0xFF = none
    uint16_t soundStartDelayMs;
    uint16_t checksum;
} __attribute__((packed));
