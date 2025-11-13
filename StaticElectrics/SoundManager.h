#pragma once

#include "BoatElectronicsConstants.h"

struct SoundPattern
{
	const uint16_t* durations;  // Array of blast durations in ms
	uint8_t blastCount;         // Number of blasts in the pattern
	uint32_t repeatInterval;    // 0 = no repeat, else ms between repetitions
};

enum class SoundState : uint8_t
{
	Idle,
	BlastOn,
	BlastGap,
	WaitingRepeat
};

class SoundManager
{
private:
	bool _isPlaying;
	SoundType _soundType;
	SoundState _state;
	
	uint8_t _currentBlastIndex;
	unsigned long _stateStartTime;
	unsigned long _patternStartTime;
	
	const SoundPattern* _currentPattern;
	
	void startPattern(const SoundPattern* pattern);
	void stopPattern();
	const SoundPattern* getPattern(SoundType soundType) const;
	
public:
	SoundManager();
	void playSound(const SoundType soundType);
	bool isPlaying() const { return _isPlaying; }
	void update(unsigned long currentTime);
};