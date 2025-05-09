#ifndef I_LED_STRIP_H
#define I_LED_STRIP_H

#include <Arduino.h>

class ILEDStrip
{
public:
    virtual void begin() = 0;
    virtual void setBrightness(uint8_t) = 0;
    virtual uint16_t numPixels() = 0;
    virtual void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void show() = 0;
};

#endif // I_LED_STRIP_H