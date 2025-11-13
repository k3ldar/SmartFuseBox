#pragma once

#include <Arduino.h>

enum class SoundType : uint8_t
{
	None = 0x00,
	Sos = 0x01,
	Fog = 0x02,
	MoveStarboard = 0x03,
	MovePort = 0x04,
	MoveAstern = 0x05,
	MoveDanger = 0x06,
	OvertakeStarboard = 0x07,
	OvertakePort = 0x08,
	OvertakeConsent = 0x09,
	OvertakeDanger = 0x0A,
	Test = 0x0B
};

// COLREGS Sound Signal Durations
constexpr uint16_t SHORT_BLAST_MS = 1000;  // ~1 second (COLREGS Rule 34)
constexpr uint16_t LONG_BLAST_MS = 5000;   // 4-6 seconds (COLREGS Rule 34)
constexpr uint16_t BLAST_GAP_MS = 1000;    // Gap between blasts in a sequence

// COLREGS Repeat Intervals
constexpr uint32_t FOG_REPEAT_MS = 120000;  // 2 minutes (COLREGS Rule 35)
constexpr uint32_t SOS_REPEAT_MS = 10000;   // 10 seconds (distress signal)
constexpr uint32_t NO_REPEAT = 0;           // One-shot signals