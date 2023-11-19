#ifndef GORKON_HPP
#define GORKON_HPP

#include "GkCCEncoder.hpp"
#include "GkCCButton.hpp"
#include "sysex_proto.h"

template <uint8_t NbEnc, uint8_t NbBtn>
class Gorkon : MIDI_Callbacks
{
public:
    Gorkon( const uint8_t (&enc_pin)[NbEnc], const uint8_t (&enc_mcc)[NbEnc],
            const uint8_t (&btn_pin)[NbBtn], const uint8_t (&btn_mcc)[NbBtn],
            const bool    (&btn_tog)[NbBtn]) :
        channel(0)
    {
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
    };

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
    void sendPatchStatus();
    void saveConfig();
    void restoreConfig();
    void resetConfig();

#ifdef GK_DEBUG
    void dumpConfig();
#endif

    // MIDI_Callbacks
    void onSysExMessage(MIDI_Interface &, SysExMessage sysex) override;


private:
    uint8_t enc_pin[NbEnc];
    uint8_t default_enc_mcc[NbEnc];
    
    uint8_t btn_pin[NbBtn];
    uint8_t default_btn_mcc[NbBtn];
    bool    default_btn_tog[NbBtn];

    GkCCEncoder* enc[NbEnc];
    GkCCButton*  btn[NbBtn];

    uint8_t channel;
};

#include "Gorkon.tpp"
#endif // GORKON_HPP