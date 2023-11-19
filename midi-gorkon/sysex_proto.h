#ifndef SYSEX_PROTO_H
#define SYSEX_PROTO_H

namespace SysExProto {
    constexpr static const uint8_t SysExStart = 0xF0;
    constexpr static const uint8_t SysExEnd   = 0xF7;

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

    template<uint8_t NbEnc, uint8_t NbBtn>
    union patch_sts_u
    {
        struct msg
        {
            uint8_t syx_hdr; // 0xF0
            uint8_t msg_idx;
            uint8_t enc_mcc[NbEnc];
            btn_t   btn_cfg[NbBtn];
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
};

#endif // SYSEX_PROTO_H