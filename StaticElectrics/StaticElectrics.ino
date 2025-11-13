#include <Arduino.h>
#include <stdint.h>
#include <UnoWiFiDevEd.h>
#include <Adafruit_Sensor.h>
#include <dht11.h>
#include <SerialCommandManager.h>

#include "Queue.h"
#include "BoatElectronicsConstants.h"
#include "SoundManager.h"
#include "SoundCommandHandler.h"
#include "RelayCommandHandler.h"
#include "BaseCommandHandler.h"


#define COMPUTER_SERIAL Serial
#define LINK_SERIAL Serial1

const int DefaultDelay = 5;

const int WaterSensorPin = A0;
const int WaterSensorActivePin = D8;

const int SensorCheckIntervalMs = 5000;

const unsigned long serialInitTimeoutMs = 300;
const unsigned long serialReconnectMs = 10000;

const int TempSensorPin = D9;

// Digital pins for relays
const int Relay4 = D4;
const int Relay3 = D5;
const int Relay2 = D6;
const int Relay1 = D7;

// Analog pins used as digital (A2–A5 → 16–19)
const int Relay8 = 16;
const int Relay7 = 17;
const int Relay6 = 18;
const int Relay5 = 19;

const int TotalRelays = 8;

int Relays[TotalRelays] = { Relay1, Relay2, Relay3, Relay4, Relay5, Relay6, Relay7, Relay8 };

// forward declares
void InitializeSerial(HardwareSerial& serialPort, unsigned long baudRate, bool waitForConnection = false);
void reconnectSerial(unsigned long now);
void onComputerCommandReceived(SerialCommandManager* mgr);
void onLinkCommandReceived(SerialCommandManager* mgr);

SerialCommandManager commandMgrComputer(&COMPUTER_SERIAL, onComputerCommandReceived, '\n', ':', '=', 500, 64);
SerialCommandManager commandMgrLink(&LINK_SERIAL, onLinkCommandReceived, '\n', ':', '=', 500, 64);

SoundManager soundManager;

RelayCommandHandler relayHandler(&commandMgrComputer, &commandMgrLink, Relays, TotalRelays);
SoundCommandHandler soundHandler(&commandMgrComputer, &commandMgrLink, &soundManager);

unsigned long nextWaterSensorCheck = 5000;
Queue waterPumpQueue(15);

dht11 dht11Sensor;

unsigned long nextWeatherSensorCheck = 2500;
unsigned long lastSerialConnectAttempt = 0;



void setup()
{
    ISerialCommandHandler* linkHandlers[] = { &relayHandler, &soundHandler } ;
    size_t linkHandlerCount = sizeof(linkHandlers) / sizeof(linkHandlers[0]);
    commandMgrLink.registerHandlers(linkHandlers, linkHandlerCount);

    ISerialCommandHandler* computerHandlers[] = { &relayHandler, &soundHandler };
    size_t computerHandlerCount = sizeof(computerHandlers) / sizeof(computerHandlers[0]);
    commandMgrComputer.registerHandlers(computerHandlers, computerHandlerCount);

    InitializeSerial(COMPUTER_SERIAL, 115200, true);
    InitializeSerial(LINK_SERIAL, 9600, true);

    commandMgrComputer.sendCommand("INIT", "Initializing");

    // water sensor
    pinMode(WaterSensorActivePin, OUTPUT);
    digitalWrite(WaterSensorActivePin, LOW);
    relayHandler.setup();

    commandMgrComputer.sendCommand("INIT", "Initialized");
}

void loop() 
{
    unsigned long now = millis();
    reconnectSerial(now);
    commandMgrComputer.readCommands();
    commandMgrLink.readCommands();
	soundManager.update(now);

    getWaterSensorValue(now);
    readDHT11Sensor(now);

    delay(DefaultDelay);
}

void getWaterSensorValue(unsigned long currTime)
{
    if (currTime > nextWaterSensorCheck)
    {
        if (waterPumpQueue.isFull())
          waterPumpQueue.dequeue();

        digitalWrite(WaterSensorActivePin, HIGH);

        delay(10);
        int sensorValue = analogRead(WaterSensorPin);
        waterPumpQueue.enqueue(sensorValue);

        digitalWrite(WaterSensorActivePin, LOW);

        nextWaterSensorCheck = currTime + SensorCheckIntervalMs;
        commandMgrLink.sendCommand("WTR", String(waterPumpQueue.average()));

        commandMgrComputer.sendDebug(String(sensorValue), "WTRLVL");
        commandMgrComputer.sendDebug(String(waterPumpQueue.average()), "WTRAVG");
    }
}

void readDHT11Sensor(unsigned long currTime)
{
    if (currTime > nextWeatherSensorCheck)
    {
        dht11Sensor.read(TempSensorPin);
        commandMgrComputer.sendDebug(String(dht11Sensor.humidity, 1), "Humidity");
        commandMgrComputer.sendDebug(String(dht11Sensor.temperature, 1), "Temperature");
        float humidity = dht11Sensor.humidity;
        float tempCelsius = dht11Sensor.temperature;

        nextWeatherSensorCheck = currTime + SensorCheckIntervalMs;

        commandMgrLink.sendCommand("TMP", String(tempCelsius, 1));
        commandMgrLink.sendCommand("HUM", String(humidity, 0));
    }
}

void onComputerCommandReceived(SerialCommandManager* mgr)
{
    commandMgrComputer.sendError(mgr->getRawMessage(), "STATCMD");
}

void onLinkCommandReceived(SerialCommandManager* mgr)
{
    commandMgrComputer.sendError(mgr->getRawMessage(), "STATLNK");
}

void InitializeSerial(HardwareSerial& serialPort, unsigned long baudRate, bool waitForConnection)
{
    serialPort.begin(baudRate);

    if (waitForConnection)
    {
        unsigned long leave = millis() + serialInitTimeoutMs;

        while (!serialPort && millis() < leave)
            delay(10);
    }
}

void reconnectSerial(unsigned long now)
{
    if (now < lastSerialConnectAttempt + serialReconnectMs)
        return;

    lastSerialConnectAttempt = now;

    if (!COMPUTER_SERIAL)
    {
        InitializeSerial(COMPUTER_SERIAL, 115200, true);
    }

    if (!LINK_SERIAL)
    {
        InitializeSerial(LINK_SERIAL, 9600, true);
    }
}