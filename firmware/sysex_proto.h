#ifndef SYSEX_PROTO_H
#define SYSEX_PROTO_H

namespace SysExProto {
    constexpr static const uint8_t SysExStart = 0xF0;
    constexpr static const uint8_t SysExEnd   = 0xF7;
    constexpr static const uint8_t ManuId     = 0x7D;

    enum
    {
        PATCH_REQ,             // In:  Request for current configuration
        PATCH_STS,             // Out: Send configuration
        PATCH_ENC_CMD,         // In:  Change an encoder patch
        PATCH_BTN_CMD,         // In:  Change a button patch
        TOGGLE_BTN_CMD,        // In:  Change a button toggle
        CHANGE_CHAN_CMD,       // In:  Change the midi channel
        CHANGE_START_NOTE_CMD, // In:  Change the piano start note
        SAVE_CMD,              // In:  Save current configuration command
        RESET_CMD              // In:  Restore default configuration
    };

    typedef struct
    {
        uint8_t mcc; // MIDI CC
        bool    tog; // Is toggle button
    } btn_t;

    typedef struct svt
    {
        uint8_t  major;
        uint8_t  minor;
        uint16_t patch;
    } semver_t;

    template<uint8_t NbEnc, uint8_t NbBtn>
    union patch_sts_u
    {
        struct msg
        {
            uint8_t syx_hdr; // 0xF0
            uint8_t manu_id; // Manufacturer ID 7D: prototyping a,d private use.
            uint8_t msg_idx;
            semver_t fw_ver;
            uint8_t channel;
            uint8_t enc_mcc[NbEnc];
            btn_t   btn_cfg[NbBtn];
            uint8_t st_note;
            uint8_t syx_ftr; // 0xF7
        } sts;
        uint8_t array[sizeof(struct msg)];
    };


    typedef struct
    {
        uint8_t syx_hdr; // 0xF0
        uint8_t msg_idx;
        uint8_t ctl_idx;
        uint8_t ctl_val;
        uint8_t syx_ftr; // 0xF7
    } patch_cmd_t;

    typedef struct
    {
        uint8_t syx_hdr; // 0xF0
        uint8_t msg_idx;
        uint8_t channel;
        uint8_t syx_ftr; // 0xF7
    } change_chan_cmd_t;

    typedef struct
    {
        uint8_t syx_hdr; // 0xF0
        uint8_t msg_idx;
        uint8_t st_note;
        uint8_t syx_ftr; // 0xF7
    } change_start_note_cmd_t;
};

#endif // SYSEX_PROTO_H