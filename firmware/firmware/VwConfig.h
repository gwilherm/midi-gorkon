#ifndef VW_CONFIG_H
#define VW_CONFIG_H

#include <Control_Surface.h>

#include "hardware_config.h"

#define DEFAULT_MIDI_CHANNEL 0

// Encoders default MIDI CC
const uint8_t DEFAULT_ENC_MIDI_CC[ENC_NB] = { MIDI_CC::Sound_Controller_2, MIDI_CC::Sound_Controller_3, MIDI_CC::Portamento_Time,  MIDI_CC::Effects_1, 
                                              MIDI_CC::Sound_Controller_5, MIDI_CC::Sound_Controller_4, MIDI_CC::Effects_4,        MIDI_CC::Pan };


// Buttons default MIDI CC
const uint8_t DEFAULT_BTN_MIDI_CC[BTN_NB] = { MIDI_CC::Portamento };

// Buttons default toggle
const bool DEFAULT_BTN_TOGGLE[BTN_NB] = { true };

class VwConfig
{
public:
    VwConfig(): _channel(DEFAULT_MIDI_CHANNEL) {};
    virtual ~VwConfig() = default;

    void setChannel(uint8_t channel) { _channel = channel; };
    uint8_t getChannel() { return _channel; };

protected:
    uint8_t _channel;
};

#endif // VW_CONFIG_H