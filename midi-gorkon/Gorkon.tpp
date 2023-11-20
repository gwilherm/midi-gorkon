#include <Control_Surface.h>
#include <EEPROM.h>

#include "Gorkon.hpp"

#define SDO_PIN 2
#define SCL_PIN 3
#define RGB_PIN 4
#define BTN_PIN 16
#define LED_COUNT 4

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use

template <uint8_t NbEnc, uint8_t NbBtn>
Gorkon<NbEnc, NbBtn>::
Gorkon( const uint8_t (&enc_pin)[NbEnc], const uint8_t (&enc_mcc)[NbEnc],
        const uint8_t (&btn_pin)[NbBtn], const uint8_t (&btn_mcc)[NbBtn],
        const bool    (&btn_tog)[NbBtn]):
    piano(SCL_PIN, SDO_PIN, MIDI_Notes::C(4)),
    pianoRGB(LED_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800),
    channel(0)
{
    char *copy = strdup(GK_VERSION);

    this->fw_version = {
        .major =  static_cast<uint8_t>(atoi(strtok(copy, "."))),
        .minor =  static_cast<uint8_t>(atoi(strtok(NULL, "."))),
        .patch = static_cast<uint16_t>(atoi(strtok(NULL, ".")))
    };

    for (int i = 0; i < NbEnc; i++)
    {
        this->enc_pin[i] = enc_pin[i];
        this->default_enc_mcc[i] = enc_mcc[i];
    }
    for (int i = 0; i < NbBtn; i++)
    {
        this->btn_pin[i] = btn_pin[i];
        this->default_btn_mcc[i] = btn_mcc[i];
        this->default_btn_tog[i] = btn_tog[i];
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::begin()
{
    restoreConfig();

#ifdef GK_DEBUG
    dumpConfig();
#endif

    pinMode(BTN_PIN, INPUT_PULLUP);

    pianoRGB.begin();           // INITIALIZE NeoPixel pianoRGB object (REQUIRED)
    pianoRGB.show();            // Turn OFF all pixels ASAP
    pianoRGB.setBrightness(10);

    Control_Surface.begin();  // Initialize the Control Surface
    midi.begin();
    midi.setCallbacks(this);
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::update()
{   
    handlePianoModeSwitch();

    Control_Surface.loop();  // Update the Control Surface

    midi.update();
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::sendPatchStatus()
{
    Serial << "Firmware version: " << GK_VERSION << endl;

    SysExProto::patch_sts_u<NbEnc, NbBtn> sts;
    sts.sts.syx_hdr = SysExProto::SysExStart;
    sts.sts.syx_ftr = SysExProto::SysExEnd;
    sts.sts.manu_id = SysExProto::ManuId;
    sts.sts.msg_idx = SysExProto::PATCH_STS;

    sts.sts.fw_ver  = this->fw_version;
    sts.sts.channel = this->channel;

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

    sts.sts.st_note = this->piano.getBaseAddress().getAddress();

    midi.sendSysEx(sts.array);
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::handleChangeChannelSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::change_chan_cmd_t))
    {
        uint8_t newChan = ((SysExProto::change_chan_cmd_t*)msg)->channel;
        if (newChan <= 15)
        {
#ifdef GK_DEBUG
            Serial << "Channel = " << newChan << endl;
#endif
            // Change all component channel
            for (int i = 0; i < NbEnc; i++)
                if (this->enc[i])
                    this->enc[i]->setAddress({
                        this->enc[i]->getAddress().getAddress(),
                        Channel(newChan)
                    });

            for (int i = 0; i < NbBtn; i++)
                if (this->btn[i])
                    this->btn[i]->setAddressUnsafe({
                        this->btn[i]->getAddress().getAddress(),
                        Channel(newChan)
                    });

            this->piano.setBaseAddressUnsafe({
                this->piano.getBaseAddress().getAddress(),
                Channel(newChan)
            });

            this->channel = newChan;
        }
    }
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::handleChangeStartNoteSysEx(const uint8_t* msg, unsigned size)
{
    if (size == sizeof(SysExProto::change_start_note_cmd_t))
    {
        uint8_t startNote = ((SysExProto::change_start_note_cmd_t*)msg)->st_note;
        if (startNote <= 127)
        {
#ifdef GK_DEBUG
            Serial << "Start note = " << startNote << endl;
#endif

            this->piano.setBaseAddressUnsafe({startNote, Channel(this->channel)});
        }
    }
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
    EEPROM.update(0, this->channel);

    for (int i = 0; i < NbEnc; i++)
    {
        if (this->enc[i])
            EEPROM.update(1+i, this->enc[i]->getAddress().getAddress());
    }

    for (int i = 0; i < NbBtn; i++)
    {
        uint8_t mcc = default_btn_mcc[i];
        uint8_t tog = default_btn_tog[i];
        if (this->btn[i])
        {
            mcc = this->btn[i]->getAddress().getAddress();
            tog = this->btn[i]->isToggle();
        }
        EEPROM.update(1+NbEnc+i,   mcc);
        EEPROM.update(1+NbEnc+i+1, tog);
    }
#ifdef GK_DEBUG
    Serial << "Config saved." << endl;
#endif
}

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::restoreConfig()
{
    this->channel = EEPROM.read(0);

    for (int i = 0; i < NbEnc; i++)
    {
        uint8_t mcc = EEPROM.read(1+i);
        if (mcc > 127)
            mcc = default_enc_mcc[i];

        this->enc[i] = new GkCCEncoder(enc_pin[i], mcc);
    }

    for (int i = 0; i < NbBtn; i++)
    {
        uint8_t mcc = EEPROM.read(1+NbEnc+i);
        uint8_t tog = EEPROM.read(1+NbEnc+i+1);
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
    this->channel = 0;

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

#ifdef GK_DEBUG
template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::dumpConfig()
{
    Serial << "Channel = " << this->channel << endl;
    for (int i = 0; i < NbEnc; i++)
    {
        Serial << "Enc[" << i << "] (pin:" << enc_pin[i] << ")"
            << " => MCC = " << this->enc[i]->getAddress().getAddress()
            << endl;
    }

    for (int i = 0; i < NbBtn; i++)
    {
        Serial << "Btn[" << i << "] (pin:" << btn_pin[i] << ")"
            << " => MCC = " << this->btn[i]->getAddress().getAddress()
            << " -- TOG = " << this->btn[i]->isToggle()
            << endl;
    }
}
#endif

// This callback function is called when a SysEx message is received.
template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::onSysExMessage(MIDI_Interface &, SysExMessage sysex)
{
#ifdef GK_DEBUG
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

template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::handlePianoModeSwitch()
{
    int newBtnState = digitalRead(BTN_PIN);
    if ((millis() - this->pianoModeSwitchLastDebounceTime) > 70)
    {
        if ((this->pianoModeSwitchState == LOW) && (newBtnState == HIGH))
        {
            switch (piano.getMode())
            {
            case PianoMode::Standard:
#ifdef GK_DEBUG
                Serial.println("Hold");
#endif
                piano.setMode(PianoMode::Hold);
                pianoRGBColorFade( 25, 210,  25); // fade into green
                break;
            case PianoMode::Hold:
#ifdef GK_DEBUG
                Serial.println("Monodic");
#endif
                piano.setMode(PianoMode::Monodic);
                pianoRGBColorFade( 25,  25, 210); // fade into blue
                break;
            case PianoMode::Monodic:
#ifdef GK_DEBUG
                Serial.println("Standard");
#endif
                piano.setMode(PianoMode::Standard);
                pianoRGBColorFade(  0,   0,   0); // fade into black
                break;
            }
        }
        this->pianoModeSwitchState = newBtnState;
        this->pianoModeSwitchLastDebounceTime = millis();
    }
}


template <uint8_t NbEnc, uint8_t NbBtn>
void Gorkon<NbEnc, NbBtn>::pianoRGBColorFade(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t curr_r, curr_g, curr_b;
    uint32_t curr_col = pianoRGB.getPixelColor(0); // get the current colour
    curr_b = curr_col & 0xFF; curr_g = (curr_col >> 8) & 0xFF; curr_r = (curr_col >> 16) & 0xFF; // separate into RGB components

    while ((curr_r != r) || (curr_g != g) || (curr_b != b)){  // while the curr color is not yet the target color
        if (curr_r < r) curr_r++; else if (curr_r > r) curr_r--;  // increment or decrement the old color values
        if (curr_g < g) curr_g++; else if (curr_g > g) curr_g--;
        if (curr_b < b) curr_b++; else if (curr_b > b) curr_b--;

        for(uint16_t i = 0; i < pianoRGB.numPixels(); i++)
            pianoRGB.setPixelColor(i, curr_r, curr_g, curr_b);  // set the color

        pianoRGB.show();
        delay(1);  // add a delay if its too fast
    }
}