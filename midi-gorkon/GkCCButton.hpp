#ifndef GK_CC_BUTTON_HPP
#define GK_CC_BUTTON_HPP

#include "midi_addressable.h"

class GkCCButton
{
public:
    GkCCButton(uint8_t pin, uint8_t mcc, bool toggle):
        addressable(nullptr),
        _toggle(toggle)
    {
        if (_toggle)
            addressable = make_addressable<CCButtonLatched>(pin, mcc);
        else
            addressable = make_addressable<CCButton>(pin, mcc);
    }

    virtual ~GkCCButton()
    {
        delete addressable;
    }

    inline MIDIAddress getAddress() { return addressable->getAddress(); }
    inline void setAddressUnsafe(MIDIAddress address)
        { addressable->setAddressUnsafe(address); }

    inline bool isToggle()             { return _toggle; }
    inline void setToggle(bool toggle) { _toggle = toggle; }
private:
    MIDIAddressable* addressable;
    bool _toggle;
};

#endif // GK_CC_BUTTON_HPP