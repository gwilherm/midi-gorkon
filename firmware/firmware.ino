#include "Gorkon.hpp"

// Number of encoders (XY = 2 encoders)
#define ENC_NB 8
// Number of buttons
#define BTN_NB 1
                            // Encoders pins
Gorkon<ENC_NB, BTN_NB> gk(  { A10, A9, A8, A7,
                              A0,  A1, A2, A3 },
                            // Encoders default MIDI CC
                            { MIDI_CC::Sound_Controller_2, MIDI_CC::Sound_Controller_3, MIDI_CC::Portamento_Time,  MIDI_CC::Effects_1, 
                              MIDI_CC::Sound_Controller_5, MIDI_CC::Sound_Controller_4, MIDI_CC::Effects_4,        MIDI_CC::Pan },
                            // Buttons pins
                            { 5 },
                            // Buttons default MIDI CC
                            { MIDI_CC::Portamento },
                            // Buttons default toggle
                            { true }
);

void setup()
{
    Serial.begin(9600);
#ifdef FW_DEBUG
    while (!Serial);

    Serial.println("setup");
#endif
    gk.begin();
}

void loop()
{
#ifdef FW_DEBUG_VERBOSE
    Serial.println("loop");
#endif
    gk.update();

}
