#ifndef VW_CC_BUTTON_H
#define VW_CC_BUTTON_H

#include "midi_addressable.h"

/**
 * @brief   A CCButton that can be either momentary or latched
 */
class CCPushButton
{
public:
    CCPushButton(uint8_t pin, uint8_t mcc, bool isToggleButton):
        addressable(nullptr),
        _isToggleButton(isToggleButton)
    {
        if (_isToggleButton)
            addressable = make_addressable<CCButtonLatched>(pin, mcc);
        else
            addressable = make_addressable<CCButton>(pin, mcc);
    }

    virtual ~CCPushButton()
    {
        delete addressable;
    }

    inline MIDIAddress getAddress() { return addressable->getAddress(); }
    inline void setAddressUnsafe(MIDIAddress address)
        { addressable->setAddressUnsafe(address); }

    inline bool isToggle()             { return _isToggleButton; }
    inline void setToggle(bool toggle) { _isToggleButton = toggle; }
private:
    MIDIAddressable* addressable;
    bool _isToggleButton;
};

#endif // VW_CC_BUTTON_H