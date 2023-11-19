#include <Control_Surface.h>
#include <EEPROM.h>

#include "Gorkon.hpp"

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::begin()
{
    restoreConfig();

    Control_Surface.begin();  // Initialize the Control Surface
    midi.begin();
    midi.setCallbacks(this);
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::update()
{
  Control_Surface.loop();  // Update the Control Surface

  midi.update();
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::sendPatchStatus()
{
    SysExProto::patch_sts_u<NbEnc, NbBtn> sts;
    sts.sts.syx_hdr = SysExProto::SysExStart;
    sts.sts.syx_ftr = SysExProto::SysExEnd;
    sts.sts.msg_idx = SysExProto::PATCH_STS;

    for (int i = 0; i < NbEnc; i++)
        if (this->enc[i])
            sts.sts.enc_mcc[i] = this->enc[i]->getAddress().getAddress();

    for (int i = 0; i < NbBtn; i++)
    {
        if (this->btn[i])
        {
            sts.sts.btn_cfg[i].mcc = this->btn[i]->getAddress().getAddress();
            sts.sts.btn_cfg[i].tog = this->btn[i]->isToggle();
        }
    }

    midi.sendSysEx(sts.array);
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::handleEncPatchSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::patch_cmd_t))
    {
        SysExProto::patch_cmd_t* patch = (SysExProto::patch_cmd_t*)msg;
        if ((patch->ctl_idx < NbEnc) && (patch->ctl_val <= 127))
            if (this->enc[patch->ctl_idx])
            this->enc[patch->ctl_idx]->setAddress(patch->ctl_val);
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::handleBtnPatchSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::patch_cmd_t))
    {
        SysExProto::patch_cmd_t* patch = (SysExProto::patch_cmd_t*)msg;
        if ((patch->ctl_idx < NbBtn) && (patch->ctl_val <= 127))
            if (this->btn[patch->ctl_idx])
                this->btn[patch->ctl_idx]->setAddressUnsafe(patch->ctl_val);
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::handleBtnToggleSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::patch_cmd_t))
    {
        SysExProto::patch_cmd_t* patch = (SysExProto::patch_cmd_t*)msg;
        if ((patch->ctl_idx < NbBtn) && (patch->ctl_val <= 1))
        {
            uint8_t mcc = default_btn_mcc[patch->ctl_idx];
            if (this->btn[patch->ctl_idx])
            {
                mcc = this->btn[patch->ctl_idx]->getAddress().getAddress();
                delete this->btn[patch->ctl_idx];
            }

            bool tog = (bool)patch->ctl_val;
            this->btn[patch->ctl_idx] = new GkCCButton(btn_pin[patch->ctl_idx], mcc, tog);
        }
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::saveConfig()
{
    for (int i = 0; i < NbEnc; i++)
    {
        if (this->enc[i])
            EEPROM.update(i, this->enc[i]->getAddress().getAddress());
    }

    for (int i = 0; i < NbBtn; i++)
    {
        uint8_t mcc = default_btn_mcc[i];
        if (this->btn[i])
            mcc = this->btn[i]->getAddress().getAddress();
        EEPROM.update(NbEnc+i,   mcc);
        EEPROM.update(NbEnc+i+1, btn[i]->isToggle());
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::restoreConfig()
{
    for (int i = 0; i < NbEnc; i++)
    {
        uint8_t mcc = EEPROM.read(i);
        if (mcc > 127)
            mcc = default_enc_mcc[i];

        this->enc[i] = new GkCCEncoder(enc_pin[i], mcc);
    }

    for (int i = 0; i < NbBtn; i++)
    {
        uint8_t mcc = EEPROM.read(NbEnc+i);
        uint8_t tog = EEPROM.read(NbEnc+i+1);
        if (mcc > 127)
            mcc = default_btn_mcc[i];
        if (tog > 127)
            tog = default_btn_tog[i];

        this->btn[i] = new GkCCButton(btn_pin[i], mcc, tog);
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::resetConfig()
{
    for (int i = 0; i < NbEnc; i++)
        if (this->enc[i])
            this->enc[i]->setAddress(default_enc_mcc[i]);

    for (int i = 0; i < NbBtn; i++)
    {
        uint8_t mcc = default_btn_mcc[i];
        bool    tog = default_btn_tog[i];
        this->btn[i] = new GkCCButton(btn_pin[i], mcc, tog);
    }
}

// This callback function is called when a SysEx message is received.
template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::onSysExMessage(MIDI_Interface &, SysExMessage sysex) {
#ifdef GK_DEBUG
    // Print the message
    Serial << F("Received SysEx message: ")         //
            << AH::HexDump(sysex.data, sysex.length) //
            << F(" on cable ") << sysex.cable.getOneBased() << endl;
#endif

    if (sysex.length < 1) return;

    switch (sysex.data[1])
    {
    case SysExProto::PATCH_REQ:
        sendPatchStatus();
        break;
    case SysExProto::PATCH_ENC_CMD:
        handleEncPatchSysEx(sysex.data, sysex.length);
        break;
    case SysExProto::PATCH_BTN_CMD:
        handleBtnPatchSysEx(sysex.data, sysex.length);
        break;
    case SysExProto::TOGGLE_BTN_CMD:
        handleBtnToggleSysEx(sysex.data, sysex.length);
        break;
    case SysExProto::SAVE_CMD:
        saveConfig();
        break;
    case SysExProto::RESET_CMD:
        resetConfig();
        break;
    default:
        break;
    }
}