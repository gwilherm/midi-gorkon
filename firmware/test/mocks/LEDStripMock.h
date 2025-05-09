#ifndef LED_STRIP_MOCK_H
#define LED_STRIP_MOCK_H

#include "ILEDStrip.h"

#include <gmock/gmock.h>

class LEDStripMock : public ILEDStrip {
  public:
    LEDStripMock() = default;
    virtual ~LEDStripMock() = default;

    MOCK_METHOD(void, begin, ());
    MOCK_METHOD(void, setBrightness, (uint8_t));
    MOCK_METHOD(uint16_t, numPixels, ());
    MOCK_METHOD(void, setPixelColor, (uint16_t, uint8_t, uint8_t, uint8_t));
    MOCK_METHOD(void, show, ());
};

#endif