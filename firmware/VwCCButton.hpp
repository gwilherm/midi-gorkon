#ifndef VW_CC_BUTTON_HPP
#define VW_CC_BUTTON_HPP

#include "midi_addressable.h"

class VwCCButton
{
public:
    VwCCButton(uint8_t pin, uint8_t mcc, bool toggle):
        addressable(nullptr),
        _toggle(toggle)
    {
        if (_toggle)
            addressable = make_addressable<CCButtonLatched>(pin, mcc);
        else
            addressable = make_addressable<CCButton>(pin, mcc);
    }

    virtual ~VwCCButton()
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

#endif // VW_CC_BUTTON_HPP