#include <stdint.h>
#pragma once

#include <SerialCommandManager.h>
#include <NextionControl.h>

#include "Config.h"

const char PageName[] = "page 0";
const float CompassTemperatureWarningValue = 35;
const char ControlHumidityControl[] = "t3";
const char ControlTemperatureControl[] = "t2";
const char ControlBearingText[] = "t6";
const char ControlBearingDirection[] = "t4";
const char ControlSpeed[] = "t5";
const char ControlBoatName[] = "t0";

// Update type constants for HomePage external updates
enum class HomePageUpdateType : uint8_t {
    None = 0x00,
    RelayState = 0x01,
};

// Data structure for relay state updates
struct RelayStateUpdate {
    uint8_t relayIndex;  // 0-based relay index (0..7)
    bool isOn;           // true = relay on, false = relay off
};

class HomePage : public BaseDisplayPage {
private:
    SerialCommandManager* _commandMgrLink;
    SerialCommandManager* _commandMgrComputer;
    Config* _config;
    float _lastTemp = NAN;
    float _lastHumidity = NAN;
    float _lastBearing = NAN;
    float _lastSpeed = NAN;
    String _lastDirection = "";
    float _lastCompassTemp = NAN;
    byte _compassTempAboveNorm = 0;
    bool _dangerControlShown; 
	bool _buttonOn[HOME_BUTTONS] = { false, false, false, false };
	byte _buttonImage[HOME_BUTTONS] = { BTN_COLOR_GREY, BTN_COLOR_GREY, BTN_COLOR_GREY, BTN_COLOR_GREY };
	const byte _buttonImageOn[HOME_BUTTONS] = { BTN_COLOR_BLUE, BTN_COLOR_BLUE, BTN_COLOR_BLUE, BTN_COLOR_BLUE };

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
    uint8_t getPageId() const override { return 0; }
    void begin() override;
    void refresh() override;

    //optional overrides
    void handleTouch(uint8_t compId, uint8_t eventType) override;
    void handleText(String text) override;
    void handleExternalUpdate(uint8_t updateType, const void* data) override;

public:
    explicit HomePage(Stream* serialPort, SerialCommandManager* commandMgrLink, SerialCommandManager* commandMgrComputer);

    void configSet(Config* config);
    void configUpdated();

    // Setters for updating values
    void setTemperature(float tempC);
    void setHumidity(float humPerc);
    void setBearing(float dir);
    void setSpeed(float speedKn);
    void setDirection(String dir);
    void setCompassTemperature(float tempC);
};
