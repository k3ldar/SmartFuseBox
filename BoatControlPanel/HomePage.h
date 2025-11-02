#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>
#include <stdint.h>

#include "Config.h"
#include "BaseBoatPage.h"

class HomePage : public BaseBoatPage {
private:
    float _lastTemp = NAN;
    float _lastHumidity = NAN;
    float _lastBearing = NAN;
    float _lastSpeed = NAN;
    String _lastDirection = "";
    float _lastCompassTemp = NAN;
    byte _compassTempAboveNorm = 0;
    bool _dangerControlShown; 
	bool _buttonOn[HOME_BUTTONS] = { false, false, false, false };
	byte _buttonImage[HOME_BUTTONS] = { IMG_BTN_COLOR_GREY, IMG_BTN_COLOR_GREY, IMG_BTN_COLOR_GREY, IMG_BTN_COLOR_GREY };
	const byte _buttonImageOn[HOME_BUTTONS] = { IMG_BTN_COLOR_BLUE, IMG_BTN_COLOR_BLUE, IMG_BTN_COLOR_BLUE, IMG_BTN_COLOR_BLUE };

    uint8_t _slotToRelay[HOME_BUTTONS] = { 0xFF, 0xFF, 0xFF, 0xFF }; // map home slots 0..3 -> relay index 0..7 or 0xFF empty

    // Internal methods to update the display
    void updateTemperature();
    void updateHumidity();
    void updateBearing();
    void updateSpeed();
    void updateDirection();

    uint8_t getButtonColor(uint8_t buttonIndex, bool isOn);

protected:
    // Required overrides
    uint8_t getPageId() const override { return PAGE_HOME; }
    void begin() override;
    void refresh() override;

    //optional overrides
    void handleTouch(uint8_t compId, uint8_t eventType) override;
    void handleText(String text) override;
    void handleExternalUpdate(uint8_t updateType, const void* data) override;

public:
    explicit HomePage(Stream* serialPort,
                     WarningManager* warningMgr,
                     SerialCommandManager* commandMgrLink = nullptr,
                     SerialCommandManager* commandMgrComputer = nullptr);

    // Override configUpdated from BaseBoatPage
    void configUpdated() override;

    // Setters for updating values
    void setTemperature(float tempC);
    void setHumidity(float humPerc);
    void setBearing(float dir);
    void setSpeed(float speedKn);
    void setDirection(String dir);
    void setCompassTemperature(float tempC);
};
