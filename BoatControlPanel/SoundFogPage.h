#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>
#include <stdint.h>

#include "BaseBoatPage.h"
#include "NextionIds.h"
#include "BoatControlPanelConstants.h"


class SoundFogPage : public BaseBoatPage {
private:
    unsigned long _lastRefreshTime = 0;

protected:
    // Required overrides
    uint8_t getPageId() const override { return PageSoundFogSignals; }
    void begin() override;
    void refresh(unsigned long now) override;

    //optional overrides
    void handleTouch(uint8_t compId, uint8_t eventType) override;

public:
    explicit SoundFogPage(Stream* serialPort,
        WarningManager* warningMgr,
        SerialCommandManager* commandMgrLink = nullptr,
        SerialCommandManager* commandMgrComputer = nullptr);
};
