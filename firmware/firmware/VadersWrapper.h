#ifndef VADERSWRAPPER_H
#define VADERSWRAPPER_H

#include "LEDStrip.h"
#include "PianoLEDs.h"
#include "hardware_config.h"
#include "VwConfig.h"
#include "sysex_proto.h"
#include "components/TouchpadPiano.h"
#include "components/CCPushButton.h"

class VadersWrapper : MIDI_Callbacks
{
public:
    VadersWrapper();

public:
    void begin();
    void loop();

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

    int pianoModeSwitchState = HIGH;
    unsigned long pianoModeSwitchLastDebounceTime = 0;

public:
    VwConfig _config;
    LEDStrip _ledStrip;
    PianoLEDs _pianoLeds;
};

#endif // VADERSWRAPPER_H