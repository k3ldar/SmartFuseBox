#pragma once

#include <Arduino.h>
#include <BaseDisplayPage.h>
#include "BaseBoatPage.h"

class WarningPage : public BaseBoatPage
{
private:
    uint32_t _lastActiveWarnings;      // Cache of last active warnings bitmap
    unsigned long _lastUpdateTime;      // Timestamp of last display update
    static const unsigned long UpdateIntervalMs = 10000; // 10 seconds
    
    // Shared function to update warning display
    void updateWarningDisplay();
    
public:
    WarningPage(Stream* serialPort,
        WarningManager* warningMgr,
        SerialCommandManager* commandMgrLink,
        SerialCommandManager* commandMgrComputer);

    void begin() override;
    void onEnterPage() override;
    void refresh(unsigned long now) override;
    void handleTouch(uint8_t compId, uint8_t eventType) override;
    void handleExternalUpdate(uint8_t updateType, const void* data) override;
    
    uint8_t getPageId() const override { return PageWarning; }
};