#ifndef PIANO_LEDS_H
#define PIANO_LEDS_H

#include "ILEDStrip.h"
#include "utils/Chrono.h"

typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

class PianoLEDs
{
public:
    PianoLEDs(ILEDStrip& ledStrip):
        _ledStrip(ledStrip),
        _currentColor({0, 0, 0}),
        _targetColor({0, 0, 0}),
        _rgbFadeTimer(3)
    {};

    virtual ~PianoLEDs() = default;

    void begin();
    void loop();

    void fadeToGreen();
    void fadeToBlue();
    void fadeToBlack();

protected:
    ILEDStrip& _ledStrip;
    rgb_t _currentColor;
    rgb_t _targetColor;
    Chrono _rgbFadeTimer;
};

#endif // PIANO_LEDS_H