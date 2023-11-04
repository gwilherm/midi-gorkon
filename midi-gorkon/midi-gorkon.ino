#include <Control_Surface.h>
#include <EEPROM.h>

// Number of encoders (XY = 2 encoders)
#define ENC_NB 8

USBMIDI_Interface midi;  // Instantiate a MIDI Interface to use

uint8_t default_enc_mcc[] = {MIDI_CC::Sound_Controller_2, MIDI_CC::Sound_Controller_4, MIDI_CC::Portamento_Time,  MIDI_CC::Effects_1, 
                             MIDI_CC::Sound_Controller_5, MIDI_CC::Sound_Controller_4, MIDI_CC::Effects_4,        MIDI_CC::Pan};

CCPotentiometer enc[] = {
  { A10, 0 }, { A9, 0 }, { A8, 0 }, { A7, 0 },
  { A0,  0 }, { A1, 0 }, { A2, 0 }, { A3, 0 }
};

enum
{
  PATCH_REQ, // In:  Request for current configuration
  PATCH_STS, // Out: Send configuration
  PATCH_CMD, // In:  New patch command
  SAVE_CMD,  // In:  Save current configuration command
  RESET_CMD  // In:  Restore default configuration
};

typedef union
{
  struct msg
  {
    uint8_t syx_hdr; // 0xF0
    uint8_t msg_idx;
    uint8_t enc_mcc[ENC_NB];
    uint8_t syx_ftr; // 0xF7
  } sts;
  uint8_t array[sizeof(struct msg)];
} patch_sts_u;


typedef struct
{
  uint8_t syx_hdr; // 0xF0
  uint8_t msg_idx;
  uint8_t enc_idx;
  uint8_t enc_mcc;
  uint8_t syx_ftr; // 0xF7
} patch_cmd_t;

void sendPatchStatus()
{
  patch_sts_u sts;
  sts.sts.syx_hdr = 0xF0;
  sts.sts.syx_ftr = 0xF7;
  sts.sts.msg_idx = PATCH_STS;
  
  for (int i = 0; i < ENC_NB; i++)
    sts.sts.enc_mcc[i] = enc[i].getAddress().getAddress();

  midi.sendSysEx(sts.array);
}

void updatePatch(const uint8_t* array, unsigned size)
{
  if (size == sizeof(patch_cmd_t))
  {
    patch_cmd_t* patch = (patch_cmd_t*)array;
    if ((patch->enc_idx < ENC_NB) && (patch->enc_mcc <= 127))
      enc[patch->enc_idx].setAddress(patch->enc_mcc);
  }
}

void saveConfig()
{
  for (int i = 0; i < ENC_NB; i++)
    EEPROM.update(i, enc[i].getAddress().getAddress());
}

void restoreConfig()
{
  for (int i = 0; i < ENC_NB; i++)
  {
    uint8_t value = EEPROM.read(i);
    if (value <= 127)
      enc[i].setAddress(value);
    else
      enc[i].setAddress(default_enc_mcc[i]);
  }
}

void resetConfig()
{
  for (int i = 0; i < ENC_NB; i++)
    enc[i].setAddress(default_enc_mcc[i]);
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
      case PATCH_CMD:
        updatePatch(sysex.data, sysex.length);
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

void setup() {
  
  restoreConfig();
  
  Control_Surface.begin();  // Initialize the Control Surface
  midi.begin();
  midi.setCallbacks(callback);
}

void loop() {
  Control_Surface.loop();  // Update the Control Surface
  midi.update();
}
