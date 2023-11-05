#include <Control_Surface.h>
#include <EEPROM.h>
#include "midi_addressable.h"

// Number of encoders (XY = 2 encoders)
#define ENC_NB 8
// Number of buttons
#define BTN_NB 1

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use

uint8_t default_enc_mcc[] = {MIDI_CC::Sound_Controller_2, MIDI_CC::Sound_Controller_4, MIDI_CC::Portamento_Time,  MIDI_CC::Effects_1, 
                             MIDI_CC::Sound_Controller_5, MIDI_CC::Sound_Controller_4, MIDI_CC::Effects_4,        MIDI_CC::Pan};
uint8_t default_btn_mcc[] = {MIDI_CC::Portamento};
bool    default_btn_tog[] = {true};

uint8_t enc_pin[] = {A10, A9, A8, A7,
                     A0,  A1, A2, A3};
uint8_t btn_pin[] = {4};

CCPotentiometer* enc[ENC_NB];
MIDIAddressable* btn[BTN_NB];
bool             btn_tog[BTN_NB];

enum
{
  PATCH_REQ,      // In:  Request for current configuration
  PATCH_STS,      // Out: Send configuration
  PATCH_ENC_CMD,  // In:  Change a enc patch
  PATCH_BTN_CMD,  // In:  Change a button patch
  TOGGLE_BTN_CMD, // In:  Change a button toggle
  SAVE_CMD,       // In:  Save current configuration command
  RESET_CMD       // In:  Restore default configuration
};

typedef struct
{
  uint8_t mcc; // MIDI CC
  bool    tog; // Is toggle button
} btn_t;

typedef union
{
  struct msg
  {
    uint8_t syx_hdr; // 0xF0
    uint8_t msg_idx;
    uint8_t enc_mcc[ENC_NB];
    btn_t   btn_cfg[BTN_NB];
    uint8_t syx_ftr; // 0xF7
  } sts;
  uint8_t array[sizeof(struct msg)];
} patch_sts_u;


typedef struct
{
  uint8_t syx_hdr; // 0xF0
  uint8_t msg_idx;
  uint8_t ctl_idx;
  uint8_t ctl_val;
  uint8_t syx_ftr; // 0xF7
} patch_cmd_t;

void sendPatchStatus()
{
  patch_sts_u sts;
  sts.sts.syx_hdr = 0xF0;
  sts.sts.syx_ftr = 0xF7;
  sts.sts.msg_idx = PATCH_STS;
  
  for (int i = 0; i < ENC_NB; i++)
    if (enc[i])
      sts.sts.enc_mcc[i] = enc[i]->getAddress().getAddress();

  for (int i = 0; i < BTN_NB; i++)
  {
    if (btn[i])
    {
      sts.sts.btn_cfg[i].mcc = btn[i]->getAddress().getAddress();
      sts.sts.btn_cfg[i].tog = btn_tog[i];
    }
  }

  midi.sendSysEx(sts.array);
}

void updateEncPatch(const uint8_t* array, unsigned size)
{
  if (size == sizeof(patch_cmd_t))
  {
    patch_cmd_t* patch = (patch_cmd_t*)array;
    if ((patch->ctl_idx < ENC_NB) && (patch->ctl_val <= 127))
      if (enc[patch->ctl_idx])
        enc[patch->ctl_idx]->setAddress(patch->ctl_val);
  }
}

void updateBtnPatch(const uint8_t* array, unsigned size)
{
  if (size == sizeof(patch_cmd_t))
  {
    patch_cmd_t* patch = (patch_cmd_t*)array;
    if ((patch->ctl_idx < BTN_NB) && (patch->ctl_val <= 127))
      if (btn[patch->ctl_idx])
          btn[patch->ctl_idx]->setAddressUnsafe(patch->ctl_val);
  }
}

void updateBtnToggle(const uint8_t* array, unsigned size)
{
  if (size == sizeof(patch_cmd_t))
  {
    patch_cmd_t* patch = (patch_cmd_t*)array;
    if ((patch->ctl_idx < BTN_NB) && (patch->ctl_val <= 1))
    {
      uint8_t mcc = default_btn_mcc[patch->ctl_idx];
      if (btn[patch->ctl_idx])
      {
        mcc = btn[patch->ctl_idx]->getAddress().getAddress();
        delete btn[patch->ctl_idx];
      }

    btn_tog[patch->ctl_idx] = (bool)patch->ctl_val;
    if (btn_tog[patch->ctl_idx])
      btn[patch->ctl_idx] = make_addressable<CCButtonLatched>(btn_pin[patch->ctl_idx], mcc);
    else
      btn[patch->ctl_idx] = make_addressable<CCButton>(btn_pin[patch->ctl_idx], mcc);
    }
  }
}

void saveConfig()
{
  for (int i = 0; i < ENC_NB; i++)
  {
    if (enc[i])
      EEPROM.update(i, enc[i]->getAddress().getAddress());
  }

  for (int i = 0; i < BTN_NB; i++)
  {
    uint8_t mcc = default_btn_mcc[i];
    if (btn[i])
      mcc = btn[i]->getAddress().getAddress();
    EEPROM.update(ENC_NB+i,   mcc);
    EEPROM.update(ENC_NB+i+1, btn_tog[i]);
  }
}

void restoreConfig()
{
  for (int i = 0; i < ENC_NB; i++)
  {
    uint8_t mcc = EEPROM.read(i);
    if (mcc > 127)
      mcc = default_enc_mcc[i];
    enc[i] = new CCPotentiometer(enc_pin[i], mcc);
  }

  for (int i = 0; i < BTN_NB; i++)
  {
    uint8_t mcc = EEPROM.read(ENC_NB+i);
    uint8_t tog = EEPROM.read(ENC_NB+i+1);
    if (mcc > 127)
      mcc = default_btn_mcc[i];
    if (tog > 127)
      tog = default_btn_tog[i];

    if (tog)
      btn[i] = make_addressable<CCButtonLatched>(btn_pin[i], mcc);
    else
      btn[i] = make_addressable<CCButton>(btn_pin[i], mcc);
    
    btn_tog[i] = tog;
  }
}

void resetConfig()
{
  for (int i = 0; i < ENC_NB; i++)
    if (enc[i])
      enc[i]->setAddress(default_enc_mcc[i]);

  for (int i = 0; i < BTN_NB; i++)
  {
    uint8_t mcc = default_btn_mcc[i];
    btn_tog[i] = default_btn_tog[i];
    if (btn_tog[i])
      btn[i] = make_addressable<CCButtonLatched>(btn_pin[i], mcc);
    else
      btn[i] = make_addressable<CCButton>(btn_pin[i], mcc);
  }
}

// Custom MIDI callback that prints incoming SysEx messages.
struct MyMIDI_Callbacks : MIDI_Callbacks {
 
  // This callback function is called when a SysEx message is received.
  void onSysExMessage(MIDI_Interface &, SysExMessage sysex) override {
#ifdef DEBUG
    // Print the message
    Serial << F("Received SysEx message: ")         //
           << AH::HexDump(sysex.data, sysex.length) //
           << F(" on cable ") << sysex.cable.getOneBased() << endl;
#endif

    if (sysex.length < 1) return;
    switch (sysex.data[1])
    {
      case PATCH_REQ:
        sendPatchStatus();
        break;
      case PATCH_ENC_CMD:
        updateEncPatch(sysex.data, sysex.length);
        break;
      case PATCH_BTN_CMD:
        updateBtnPatch(sysex.data, sysex.length);
        break;
      case TOGGLE_BTN_CMD:
        updateBtnToggle(sysex.data, sysex.length);
        break;
      case SAVE_CMD:
        saveConfig();
        break;
      case RESET_CMD:
        resetConfig();
        break;
      default:
        break;
    }
  }
} callback {};

void setup()
{
  restoreConfig();
  
  Control_Surface.begin();  // Initialize the Control Surface
  midi.begin();
  midi.setCallbacks(callback);
}

void loop()
{
  Control_Surface.loop();  // Update the Control Surface
  midi.update();
}
