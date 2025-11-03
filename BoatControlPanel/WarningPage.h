#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>
#include <stdint.h>

#include "BaseBoatPage.h"

class WarningPage : public BaseBoatPage {
private:

protected:
    uint8_t getPageId() const override { return PAGE_WARNING; }
    void begin() override;
    void refresh(unsigned long now) override;

    //optional overrides
    void handleTouch(uint8_t compId, uint8_t eventType) override;
    void handleExternalUpdate(uint8_t updateType, const void* data) override;
    
public:
    explicit WarningPage(Stream* serialPort,
                        WarningManager* warningMgr,
                        SerialCommandManager* commandMgrLink = nullptr,
                        SerialCommandManager* commandMgrComputer = nullptr);
};