#pragma once

#include <cstdint>      // uint8_t, uint16_t 用
#include "USBHID.h"
#include "USB.h"    

class CustomHIDDevice_: public USBHIDDevice {
  private:
    USBHID hid;
  public:
    CustomHIDDevice_(void);
    void begin(void);
    bool SendReport(const void *data, int len);
    uint16_t _onGetDescriptor(uint8_t* buffer);
};

CustomHIDDevice_ &CustomHIDDevice();
