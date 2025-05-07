#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Control_Surface.h>

// Number of encoders (XY = 2 encoders)
#define ENC_NB 8
// Number of buttons
#define BTN_NB 1

// Encoders pins
const uint8_t ENC_PINS[ENC_NB] = { A10, A9, A8, A7,
                                    A0, A1, A2, A3 };

// Encoders default MIDI CC
const uint8_t ENC_DEFAULT_MIDI_CC[ENC_NB] = { MIDI_CC::Sound_Controller_2, MIDI_CC::Sound_Controller_3, MIDI_CC::Portamento_Time,  MIDI_CC::Effects_1, 
                                              MIDI_CC::Sound_Controller_5, MIDI_CC::Sound_Controller_4, MIDI_CC::Effects_4,        MIDI_CC::Pan };

// Buttons pins
const uint8_t BTN_PINS[BTN_NB] = { 5 };

// Buttons default MIDI CC
const uint8_t BTN_DEFAULT_MIDI_CC[BTN_NB] = { MIDI_CC::Portamento };

// Buttons default toggle
const bool BTN_DEFAULT_TOGGLE[BTN_NB] = { true };

#endif // CONFIG_H