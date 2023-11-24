#ifndef GORKON_HPP
#define GORKON_HPP

#include <Adafruit_NeoPixel.h>

#include "GkCCEncoder.hpp"
#include "GkCCButton.hpp"
#include "sysex_proto.h"

typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

template <uint8_t NbEnc, uint8_t NbBtn>
class Gorkon : MIDI_Callbacks
{
public:
    Gorkon( const uint8_t (&enc_pin)[NbEnc], const uint8_t (&enc_mcc)[NbEnc],
            const uint8_t (&btn_pin)[NbBtn], const uint8_t (&btn_mcc)[NbBtn],
            const bool    (&btn_tog)[NbBtn]);

public:
    void begin();
    void update();

    void setDefaultEncMCC(const uint8_t mcc[NbEnc]);
    void setDefaultBtnMCC(const uint8_t mcc[NbBtn]);
    void setDefaultBtnTog(const bool    tog[NbBtn]);

protected:
    void handleEncPatchSysEx(const uint8_t* array, unsigned size);
    void handleBtnPatchSysEx(const uint8_t* array, unsigned size);
    void handleBtnToggleSysEx(const uint8_t* array, unsigned size);
    void handleChangeChannelSysEx(const uint8_t* msg, unsigned size);
    void handleChangeStartNoteSysEx(const uint8_t* msg, unsigned size);
    void sendPatchStatus();
    void saveConfig();
    void restoreConfig();
    void resetConfig();
    void handlePianoModeSwitch();
    void pianoRGBColorFade();

#ifdef GK_DEBUG
    void dumpConfig();
#endif

    // MIDI_Callbacks
    void onSysExMessage(MIDI_Interface &, SysExMessage sysex) override;


private:
    SysExProto::semver_t fw_version;
    uint8_t enc_pin[NbEnc];
    uint8_t default_enc_mcc[NbEnc];
    
    uint8_t btn_pin[NbBtn];
    uint8_t default_btn_mcc[NbBtn];
    bool    default_btn_tog[NbBtn];

    GkCCEncoder*  enc[NbEnc];
    GkCCButton*   btn[NbBtn];
    TouchpadPiano piano;
    Adafruit_NeoPixel pianoRGB;

    rgb_t targetColor = {0, 0, 0};
    rgb_t currColor   = {0, 0, 0};
    AH::Timer<millis> rgbFadeTimer;

    int pianoModeSwitchState = HIGH;
    unsigned long pianoModeSwitchLastDebounceTime = 0;


    uint8_t channel;
};

#include "Gorkon.tpp"
#endif // GORKON_HPP