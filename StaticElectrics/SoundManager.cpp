#include "SoundManager.h"
#include "BoatElectronicsConstants.h"

// Define sound patterns according to COLREGS
// Maneuvering Signals (COLREGS Rule 34 - one-shot, non-repeating)
const uint16_t PATTERN_STARBOARD[] = {SHORT_BLAST_MS};
const uint16_t PATTERN_PORT[] = {SHORT_BLAST_MS, SHORT_BLAST_MS};
const uint16_t PATTERN_ASTERN[] = {SHORT_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS};
const uint16_t PATTERN_DANGER[] = {SHORT_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS};

// Narrow Channel / Overtaking Signals (COLREGS Rule 34 - one-shot)
const uint16_t PATTERN_OVERTAKE_STARBOARD[] = {LONG_BLAST_MS, LONG_BLAST_MS, SHORT_BLAST_MS};
const uint16_t PATTERN_OVERTAKE_PORT[] = {LONG_BLAST_MS, LONG_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS};
const uint16_t PATTERN_OVERTAKE_CONSENT[] = {LONG_BLAST_MS, SHORT_BLAST_MS, LONG_BLAST_MS, SHORT_BLAST_MS};

// Fog Signals (COLREGS Rule 35 - repeating every 2 minutes)
const uint16_t PATTERN_FOG[] = {LONG_BLAST_MS};

// SOS (International distress - repeating every 10 seconds)
const uint16_t PATTERN_SOS[] = {SHORT_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS, 
                                 LONG_BLAST_MS, LONG_BLAST_MS, LONG_BLAST_MS,
                                 SHORT_BLAST_MS, SHORT_BLAST_MS, SHORT_BLAST_MS};

// Test pattern
const uint16_t PATTERN_TEST[] = {SHORT_BLAST_MS};

// Pattern lookup table
const SoundPattern SOUND_PATTERNS[] = {
	{nullptr, 0, NO_REPEAT},                          // None
	{PATTERN_SOS, 9, SOS_REPEAT_MS},                  // Sos
	{PATTERN_FOG, 1, FOG_REPEAT_MS},                  // Fog
	{PATTERN_STARBOARD, 1, NO_REPEAT},                // MoveStarboard
	{PATTERN_PORT, 2, NO_REPEAT},                     // MovePort
	{PATTERN_ASTERN, 3, NO_REPEAT},                   // MoveAstern
	{PATTERN_DANGER, 5, NO_REPEAT},                   // MoveDanger
	{PATTERN_OVERTAKE_STARBOARD, 3, NO_REPEAT},       // OvertakeStarboard
	{PATTERN_OVERTAKE_PORT, 4, NO_REPEAT},            // OvertakePort
	{PATTERN_OVERTAKE_CONSENT, 4, NO_REPEAT},         // OvertakeConsent
	{PATTERN_DANGER, 5, NO_REPEAT},                   // OvertakeDanger (same as MoveDanger)
	{PATTERN_TEST, 1, NO_REPEAT}                      // Test
};

SoundManager::SoundManager()
	: _isPlaying(false), _soundType(SoundType::None), _state(SoundState::Idle),
	  _currentBlastIndex(0), _stateStartTime(0), _patternStartTime(0), _currentPattern(nullptr)
{
}

void SoundManager::playSound(const SoundType soundType)
{
	// Stop current pattern if any
	if (_isPlaying)
	{
		stopPattern();
	}

	_soundType = soundType;
	
	if (soundType == SoundType::None)
	{
		_isPlaying = false;
		return;
	}

	const SoundPattern* pattern = getPattern(soundType);
	if (pattern && pattern->durations)
	{
		startPattern(pattern);
	}
}

void SoundManager::update(unsigned long currentTime)
{
	if (!_isPlaying || !_currentPattern)
		return;

	unsigned long elapsed = currentTime - _stateStartTime;

	switch (_state)
	{
		case SoundState::BlastOn:
		{
			// Check if current blast duration is complete
			uint16_t blastDuration = _currentPattern->durations[_currentBlastIndex];
			if (elapsed >= blastDuration)
			{
				// TODO: Turn off sound output (e.g., digitalWrite(SOUND_PIN, LOW))
				
				_currentBlastIndex++;
				
				// Check if pattern is complete
				if (_currentBlastIndex >= _currentPattern->blastCount)
				{
					// Pattern complete - check if repeating
					if (_currentPattern->repeatInterval > 0)
					{
						_state = SoundState::WaitingRepeat;
						_stateStartTime = currentTime;
					}
					else
					{
						// One-shot pattern complete
						stopPattern();
					}
				}
				else
				{
					// Move to gap between blasts
					_state = SoundState::BlastGap;
					_stateStartTime = currentTime;
				}
			}
			break;
		}

		case SoundState::BlastGap:
		{
			// Wait for gap duration
			if (elapsed >= BLAST_GAP_MS)
			{
				// Start next blast
				// TODO: Turn on sound output (e.g., digitalWrite(SOUND_PIN, HIGH))
				_state = SoundState::BlastOn;
				_stateStartTime = currentTime;
			}
			break;
		}

		case SoundState::WaitingRepeat:
		{
			// Wait for repeat interval
			if (elapsed >= _currentPattern->repeatInterval)
			{
				// Restart pattern
				_currentBlastIndex = 0;
				_patternStartTime = currentTime;
				_stateStartTime = currentTime;
				_state = SoundState::BlastOn;
				// TODO: Turn on sound output (e.g., digitalWrite(SOUND_PIN, HIGH))
			}
			break;
		}

		default:
			break;
	}
}

void SoundManager::startPattern(const SoundPattern* pattern)
{
	_currentPattern = pattern;
	_currentBlastIndex = 0;
	_state = SoundState::BlastOn;
	_stateStartTime = millis();
	_patternStartTime = _stateStartTime;
	_isPlaying = true;
	
	// TODO: Turn on sound output (e.g., digitalWrite(SOUND_PIN, HIGH))
}

void SoundManager::stopPattern()
{
	// TODO: Turn off sound output (e.g., digitalWrite(SOUND_PIN, LOW))
	
	_currentPattern = nullptr;
	_currentBlastIndex = 0;
	_state = SoundState::Idle;
	_stateStartTime = 0;
	_patternStartTime = 0;
	_isPlaying = false;
	_soundType = SoundType::None;
}

const SoundPattern* SoundManager::getPattern(SoundType soundType) const
{
	uint8_t index = static_cast<uint8_t>(soundType);
	if (index < sizeof(SOUND_PATTERNS) / sizeof(SOUND_PATTERNS[0]))
	{
		return &SOUND_PATTERNS[index];
	}
	return nullptr;
}