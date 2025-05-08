#include "LEDStrip.h"

LEDStrip::LEDStrip(uint16_t n, int16_t pin):
    _neoPixel(n, pin, NEO_GRB + NEO_KHZ800)
{}

void LEDStrip::begin()
{
    _neoPixel.begin();
}

void LEDStrip::setBrightness(uint8_t brightness)
{
    _neoPixel.setBrightness(brightness);
}

uint16_t LEDStrip::numPixels()
{
    return _neoPixel.numPixels();
}

void LEDStrip::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    _neoPixel.setPixelColor(n, r, g, b);
}

void LEDStrip::show()
{
    _neoPixel.show();
}