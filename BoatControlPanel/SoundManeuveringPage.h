#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>
#include <stdint.h>

#include "Config.h"
#include "BaseBoatPage.h"
#include "NextionIds.h"
#include "BoatControlPanelConstants.h"


class SoundManeuveringPage : public BaseBoatPage {
private:
    unsigned long _lastRefreshTime = 0;

protected:
    // Required overrides
    uint8_t getPageId() const override { return PageSoundManeuveringSignals; }
    void begin() override;
    void refresh(unsigned long now) override;

    //optional overrides
    void handleTouch(uint8_t compId, uint8_t eventType) override;

public:
    explicit SoundManeuveringPage(Stream* serialPort,
        WarningManager* warningMgr,
        SerialCommandManager* commandMgrLink = nullptr,
        SerialCommandManager* commandMgrComputer = nullptr);
};
