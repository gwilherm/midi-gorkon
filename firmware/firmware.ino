#include "config.h"
#include "VadersWrapper.h"

VadersWrapper<ENC_NB, BTN_NB> vw(ENC_PINS, ENC_DEFAULT_MIDI_CC,
                                 BTN_PINS, BTN_DEFAULT_MIDI_CC, BTN_DEFAULT_TOGGLE
                                 );

void setup()
{
    Serial.begin(9600);
#ifdef FW_DEBUG
    while (!Serial);

    Serial.println("setup");
#endif
    vw.begin();
}

void loop()
{
#ifdef FW_DEBUG_VERBOSE
    Serial.println("loop");
#endif
    vw.update();

}
