#include "PianoLEDs.h"

void PianoLEDs::begin()
{
    this->_ledStrip.show();            // Turn OFF all pixels ASAP
    this->_ledStrip.setBrightness(10);
}

void PianoLEDs::loop()
{
    if(!this->_rgbFadeTimer) return;
    uint8_t r = this->_targetColor.r;
    uint8_t g = this->_targetColor.g;
    uint8_t b = this->_targetColor.b;

    if ((this->_currentColor.r != r) || (this->_currentColor.g != g) || (this->_currentColor.b != b)){  // while the curr color is not yet the target color
        if (this->_currentColor.r < r) this->_currentColor.r++; else if (this->_currentColor.r > r) this->_currentColor.r--;  // increment or decrement the old color values
        if (this->_currentColor.g < g) this->_currentColor.g++; else if (this->_currentColor.g > g) this->_currentColor.g--;
        if (this->_currentColor.b < b) this->_currentColor.b++; else if (this->_currentColor.b > b) this->_currentColor.b--;

        for(uint16_t i = 0; i < this->_ledStrip.numPixels(); i++)
            this->_ledStrip.setPixelColor(i, this->_currentColor.r, this->_currentColor.g, this->_currentColor.b);  // set the color

        this->_ledStrip.show();
    }
    this->_rgbFadeTimer.beginNextPeriod();
}

void PianoLEDs::fadeToGreen()
{
    this->_targetColor = { 25, 210,  25}; // fade into green
}

void PianoLEDs::fadeToBlue()
{
    this->_targetColor = { 25,  25, 210}; // fade into blue
}

void PianoLEDs::fadeToBlack()
{
    this->_targetColor = {  0,   0,   0}; // fade into black
}