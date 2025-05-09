#include <Control_Surface.h>
#include <EEPROM.h>

#include "VadersWrapper.h"
#include "hardware_config.h"

#define BTN_PIN 1
#define SDO_PIN 2
#define SCL_PIN 3

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use

VadersWrapper::VadersWrapper():
    piano(SCL_PIN, SDO_PIN, MIDI_Notes::C(4)),
    _ledStrip(PIANO_LED_COUNT, PIANO_RGB_PIN),
    _pianoLeds(_ledStrip)
{
    char *copy = strdup(FW_VERSION);

    this->fw_version = {
        .major = static_cast<uint8_t>(atoi(strtok(copy, "."))),
        .minor = static_cast<uint8_t>(atoi(strtok(NULL, "."))),
        .patch = static_cast<uint16_t>(atoi(strtok(NULL, ".")))
    };
}

void VadersWrapper::begin()
{
    restoreConfig();

#ifdef FW_DEBUG
    dumpConfig();
#endif

    pinMode(BTN_PIN, INPUT_PULLUP);

    this->_ledStrip.begin();
    _pianoLeds.begin();

    Control_Surface.begin();  // Initialize the Control Surface
    midi.begin();
    midi.setCallbacks(this);
}

void VadersWrapper::loop()
{   
    handlePianoModeSwitch();

    Control_Surface.loop();  // Update the Control Surface

    midi.update();

    _pianoLeds.loop();
}

void VadersWrapper::sendPatchStatus()
{
    Serial << "Firmware version: " << FW_VERSION << endl;

    SysExProto::patch_sts_u sts;
    sts.sts.syx_hdr = SysExProto::SysExStart;
    sts.sts.syx_ftr = SysExProto::SysExEnd;
    sts.sts.manu_id = SysExProto::ManuId;
    sts.sts.msg_idx = SysExProto::PATCH_STS;

    sts.sts.fw_ver  = this->fw_version;
    sts.sts.channel = this->_config.getChannel();

    for (int i = 0; i < ENC_NB; i++)
        if (this->enc[i])
            sts.sts.enc_mcc[i] = this->enc[i]->getAddress().getAddress();

    for (int i = 0; i < BTN_NB; i++)
    {
        if (this->btn[i])
        {
            sts.sts.btn_cfg[i].mcc = this->btn[i]->getAddress().getAddress();
            sts.sts.btn_cfg[i].tog = this->btn[i]->isToggle();
        }
    }

    sts.sts.st_note = this->piano.getBaseAddress().getAddress();

    midi.sendSysEx(sts.array);
}

void VadersWrapper::handleChangeChannelSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::change_chan_cmd_t))
    {
        uint8_t newChan = ((SysExProto::change_chan_cmd_t*)msg)->channel;
        if (newChan <= 15)
        {
#ifdef FW_DEBUG
            Serial << "Channel = " << newChan << endl;
#endif
            // Change all component channel
            for (int i = 0; i < ENC_NB; i++)
                if (this->enc[i])
                    this->enc[i]->setAddress({
                        this->enc[i]->getAddress().getAddress(),
                        Channel(newChan)
                    });

            for (int i = 0; i < BTN_NB; i++)
                if (this->btn[i])
                    this->btn[i]->setAddressUnsafe({
                        this->btn[i]->getAddress().getAddress(),
                        Channel(newChan)
                    });

            this->piano.setBaseAddressUnsafe({
                this->piano.getBaseAddress().getAddress(),
                Channel(newChan)
            });

            this->_config.setChannel(newChan);
        }
    }
}

void VadersWrapper::handleChangeStartNoteSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::change_start_note_cmd_t))
    {
        uint8_t startNote = ((SysExProto::change_start_note_cmd_t*)msg)->st_note;
        if (startNote <= 127)
        {
#ifdef FW_DEBUG
            Serial << "Start note = " << startNote << endl;
#endif

            this->piano.setBaseAddressUnsafe({startNote, Channel(this->_config.getChannel())});
        }
    }
}

void VadersWrapper::handleEncPatchSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::patch_cmd_t))
    {
        SysExProto::patch_cmd_t* patch = (SysExProto::patch_cmd_t*)msg;
        if ((patch->ctl_idx < ENC_NB) && (patch->ctl_val <= 127))
            if (this->enc[patch->ctl_idx])
            this->enc[patch->ctl_idx]->setAddress(patch->ctl_val);
    }
}

void VadersWrapper::handleBtnPatchSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::patch_cmd_t))
    {
        SysExProto::patch_cmd_t* patch = (SysExProto::patch_cmd_t*)msg;
        if ((patch->ctl_idx < BTN_NB) && (patch->ctl_val <= 127))
            if (this->btn[patch->ctl_idx])
                this->btn[patch->ctl_idx]->setAddressUnsafe(patch->ctl_val);
    }
}

void VadersWrapper::handleBtnToggleSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::patch_cmd_t))
    {
        SysExProto::patch_cmd_t* patch = (SysExProto::patch_cmd_t*)msg;
        if ((patch->ctl_idx < BTN_NB) && (patch->ctl_val <= 1))
        {
            uint8_t mcc = DEFAULT_BTN_MIDI_CC[patch->ctl_idx];
            if (this->btn[patch->ctl_idx])
            {
                mcc = this->btn[patch->ctl_idx]->getAddress().getAddress();
                delete this->btn[patch->ctl_idx];
            }

            bool tog = (bool)patch->ctl_val;
            this->btn[patch->ctl_idx] = new CCPushButton(BTN_PINS[patch->ctl_idx], mcc, tog);
        }
    }
}

void VadersWrapper::saveConfig()
{
    EEPROM.update(0, this->_config.getChannel());

    for (int i = 0; i < ENC_NB; i++)
    {
        if (this->enc[i])
            EEPROM.update(1+i, this->enc[i]->getAddress().getAddress());
    }

    for (int i = 0; i < BTN_NB; i++)
    {
        uint8_t mcc = DEFAULT_BTN_MIDI_CC[i];
        uint8_t tog = DEFAULT_BTN_TOGGLE[i];
        if (this->btn[i])
        {
            mcc = this->btn[i]->getAddress().getAddress();
            tog = this->btn[i]->isToggle();
        }
        EEPROM.update(1+ENC_NB+i,   mcc);
        EEPROM.update(1+ENC_NB+i+1, tog);
    }
#ifdef FW_DEBUG
    Serial << "Config saved." << endl;
#endif
}

void VadersWrapper::restoreConfig()
{
    this->_config.setChannel(EEPROM.read(0));

    for (int i = 0; i < ENC_NB; i++)
    {
        uint8_t mcc = EEPROM.read(1+i);
        if (mcc > 127)
            mcc = DEFAULT_ENC_MIDI_CC[i];

        this->enc[i] = new CCPotentiometer(ENC_PINS[i], mcc);
    }

    for (int i = 0; i < BTN_NB; i++)
    {
        uint8_t mcc = EEPROM.read(1+ENC_NB+i);
        uint8_t tog = EEPROM.read(1+ENC_NB+i+1);
        if (mcc > 127)
            mcc = DEFAULT_BTN_MIDI_CC[i];
        if (tog > 127)
            tog = DEFAULT_BTN_TOGGLE[i];

        this->btn[i] = new CCPushButton(BTN_PINS[i], mcc, tog);
    }
}

void VadersWrapper::resetConfig()
{
    this->_config.setChannel(0);

    for (int i = 0; i < ENC_NB; i++)
        if (this->enc[i])
            this->enc[i]->setAddress(DEFAULT_ENC_MIDI_CC[i]);

    for (int i = 0; i < BTN_NB; i++)
    {
        uint8_t mcc = DEFAULT_BTN_MIDI_CC[i];
        bool    tog = DEFAULT_BTN_TOGGLE[i];
        this->btn[i] = new CCPushButton(BTN_PINS[i], mcc, tog);
    }
}

#ifdef FW_DEBUG
void VadersWrapper::dumpConfig()
{
    Serial << "Channel = " << this->_config.getChannel() << endl;
    for (int i = 0; i < ENC_NB; i++)
    {
        Serial << "Enc[" << i << "] (pin:" << ENC_PINS[i] << ")"
            << " => MCC = " << this->enc[i]->getAddress().getAddress()
            << endl;
    }

    for (int i = 0; i < BTN_NB; i++)
    {
        Serial << "Btn[" << i << "] (pin:" << BTN_PINS[i] << ")"
            << " => MCC = " << this->btn[i]->getAddress().getAddress()
            << " -- TOG = " << this->btn[i]->isToggle()
            << endl;
    }
}
#endif

// This callback function is called when a SysEx message is received.
void VadersWrapper::onSysExMessage(MIDI_Interface &, SysExMessage sysex)
{
#ifdef FW_DEBUG
    // Print the message
    Serial << F("Received SysEx message: ")         //
            << AH::HexDump(sysex.data, sysex.length) //
            << F(" on cable ") << sysex.cable.getOneBased() << endl;
#endif

    if (sysex.length < 2) return;
    if (sysex.data[1] != SysExProto::ManuId) return;

    switch (sysex.data[2])
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
    case SysExProto::CHANGE_CHAN_CMD:
        handleChangeChannelSysEx(sysex.data, sysex.length);
        break;
    case SysExProto::CHANGE_START_NOTE_CMD:
        handleChangeStartNoteSysEx(sysex.data, sysex.length);
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

void VadersWrapper::handlePianoModeSwitch()
{
    int newBtnState = digitalRead(BTN_PIN);
    if ((millis() - this->pianoModeSwitchLastDebounceTime) > 70)
    {
        if ((this->pianoModeSwitchState == LOW) && (newBtnState == HIGH))
        {
            switch (piano.getMode())
            {
            case PianoMode::Standard:
#ifdef FW_DEBUG
                Serial.println("Hold");
#endif
                piano.setMode(PianoMode::Hold);
                _pianoLeds.fadeToGreen();
                break;
            case PianoMode::Hold:
#ifdef FW_DEBUG
                Serial.println("Monodic");
#endif
                piano.setMode(PianoMode::Monodic);
                _pianoLeds.fadeToBlue();
                break;
            case PianoMode::Monodic:
#ifdef FW_DEBUG
                Serial.println("Standard");
#endif
                piano.setMode(PianoMode::Standard);
                _pianoLeds.fadeToBlack();
                break;
            }
        }
        this->pianoModeSwitchState = newBtnState;
        this->pianoModeSwitchLastDebounceTime = millis();
    }
}
