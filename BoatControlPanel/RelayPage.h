#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>
#include <stdint.h>

#include "Config.h"
#include "BaseBoatPage.h"
#include "NextionIds.h"
#include "BoatControlPanelConstants.h"


class RelayPage : public BaseBoatPage {
private:
    unsigned long _lastRefreshTime = 0;
    bool _buttonOn[ConfigRelayCount] = { false, false, false, false, false, false, false, false };
    byte _buttonImage[ConfigRelayCount] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    byte _buttonImageOn[ConfigRelayCount] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    uint8_t _slotToRelay[ConfigRelayCount] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

protected:
    // Required overrides
    uint8_t getPageId() const override { return PageRelay; }
    void begin() override;
    void refresh(unsigned long now) override;

    //optional overrides
    void onEnterPage() override;
    void handleTouch(uint8_t compId, uint8_t eventType) override;
    void handleExternalUpdate(uint8_t updateType, const void* data) override;

public:
    explicit RelayPage(Stream* serialPort,
        WarningManager* warningMgr,
        SerialCommandManager* commandMgrLink = nullptr,
        SerialCommandManager* commandMgrComputer = nullptr);

    // Override configUpdated from BaseBoatPage
    void configUpdated() override;
};
