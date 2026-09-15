#pragma once
#include <stdint.h>
struct PowerStatus {
    bool ready;
    bool usb;
    bool battery;
    bool charging;
    uint16_t batteryMv;
};
bool startPower();
void updatePower();
PowerStatus getPowerStatus();
bool requestPowerOff();
bool powerOffPending();
bool powerOffDue();
void powerOff();
