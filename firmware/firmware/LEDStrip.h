#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "ILEDStrip.h"
#include <Adafruit_NeoPixel.h>

class LEDStrip : public ILEDStrip
{
public:
    LEDStrip(uint16_t n, int16_t pin);
    virtual ~LEDStrip() = default;

    virtual void begin();

    virtual void setBrightness(uint8_t);
    virtual uint16_t numPixels();
    virtual void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    virtual void show();

protected:
    Adafruit_NeoPixel _neoPixel;
};

#endif // LED_STRIP_H