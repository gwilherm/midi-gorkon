#ifndef VADERSWRAPPER_H
#define VADERSWRAPPER_H

#include <Adafruit_NeoPixel.h>

#include "hardware_config.h"
#include "VwConfig.h"
#include "sysex_proto.h"
#include "components/TouchpadPiano.h"
#include "components/CCPushButton.h"

typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

class VadersWrapper : MIDI_Callbacks
{
public:
    VadersWrapper();

public:
    void begin();
    void update();

    void setDefaultEncMCC(const uint8_t mcc[ENC_NB]);
    void setDefaultBtnMCC(const uint8_t mcc[BTN_NB]);
    void setDefaultBtnTog(const bool    tog[BTN_NB]);

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

#ifdef FW_DEBUG
    void dumpConfig();
#endif

    // MIDI_Callbacks
    void onSysExMessage(MIDI_Interface &, SysExMessage sysex) override;

private:
    SysExProto::semver_t fw_version;
    CCPotentiometer* enc[ENC_NB];
    CCPushButton*    btn[BTN_NB];
    TouchpadPiano piano;
    Adafruit_NeoPixel pianoRGB;

    rgb_t targetColor = {0, 0, 0};
    rgb_t currColor   = {0, 0, 0};
    AH::Timer<millis> rgbFadeTimer;

    int pianoModeSwitchState = HIGH;
    unsigned long pianoModeSwitchLastDebounceTime = 0;

public:
    VwConfig _config;
};

#endif // VADERSWRAPPER_H