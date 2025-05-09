#ifndef USB_MIDI_MOCK_H
#define USB_MIDI_MOCK_H

#include "IMIDIInterface.h"

#include <gmock/gmock.h>

class USBMIDIMock : public IMIDIInterface {
  public:
    USBMIDIMock() = default;
    virtual ~USBMIDIMock() = default;

    MOCK_METHOD(void, sendSysEx, (unsigned, const uint8_t*));

    MOCK_METHOD(void, setHandleSystemExclusive, (void (*fptr)(uint8_t *, unsigned)));

    MOCK_METHOD(void, sendNote, (IMIDIInterface::note_state_t, uint8_t, uint8_t));

    MOCK_METHOD(void, sendCC, (uint8_t, uint8_t, uint8_t));
};

#endif