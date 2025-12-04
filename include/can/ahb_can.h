#pragma once

#include <cstdint>
#define CAN_MSG_ID(_MSG) MSG_##_MSG##_ID

#define MSG_ADAS_AHB_DOW_ID 0x352
#define MSG_MHU_ADAS_req_ID 0x354
#define MSG_ADAS_ELK_ID 0x206
#define MSG_BCM_HighBeam_PosReq_ID 0x39d

/* Note:
 * start bit = 7, length = 8 => bit field is [7:0]
 * start bit = 39, length = 3 => bit field is [39:37]
 */
 
#define DECLARE_PACK_UNPACK(_MSG, _SIGNAL, _STARTBIT, _BITLEN)                          \
inline void pack_##_MSG##_##_SIGNAL(_MSG##_t& msg, uint32_t value) {                    \
    /* mask value to bit length */                                                      \
    value &= ((_BITLEN) >= 32 ? 0xFFFFFFFFu : ((1u << (_BITLEN)) - 1u));                \
    for (int i = 0; i < (_BITLEN); ++i) {                                               \
        std::size_t bit_index = (_STARTBIT) - i;                                        \
        std::size_t byte = bit_index / 8;                                               \
        std::size_t bit  = bit_index % 8;                                               \
        uint32_t bit_val = (value >> ((_BITLEN) - 1 - i)) & 1u;                         \
        msg.data[byte] = (uint8_t)((msg.data[byte] & ~(1u << bit)) | (bit_val << bit)); \
    }                                                                                   \
}                                                                                       \
                                                                                        \
inline uint32_t unpack_##_MSG##_##_SIGNAL(const _MSG##_t& msg) {                        \
    uint32_t value = 0;                                                                 \
    for (int i = 0; i < (_BITLEN); ++i) {                                               \
        std::size_t bit_index = (_STARTBIT) - i;                                        \
        std::size_t byte = bit_index / 8;                                               \
        std::size_t bit  = bit_index % 8;                                               \
        value = (value << 1) | ((msg.data[byte] >> bit) & 1u);                          \
    }                                                                                   \
    return value;                                                                       \
}

struct ADAS_AHB_DOW_t {
    uint8_t data[8] = {0};
};


DECLARE_PACK_UNPACK(ADAS_AHB_DOW, CHKSM_ADAS_AHB_DOW, 7, 8) //  Checksum
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_AHB_Mode_Feed, 14, 1) //  Auto headlight high beam On off Feedback
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_AHB_sens_Feed, 13, 2) //  Auto headlight high beam sensitivity Feedback
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ALV_ADAS_AHB_DOW, 11, 4) //  Alive Counter
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_DOW_SndWarn, 23, 2) //  Door Open Sound Warning en disable
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_DOW_WarnRight, 21, 2) //  Right Door Open Warning
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_AHB_state, 19, 2) //  Auto headlight high beam control State
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_AHB_check, 17, 2) //  Auto headlight high beam control Check
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_DOW_WarnLeft, 31, 2) //  Left Door Open Warning
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_DOW_check, 29, 2) //  Door Opening Warning Check
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_DOW_state, 26, 3) //  Door Open Warning State
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_AHB_LBPos, 39, 3) //  Left High Beam Position Request Signal
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_AHB_RBPos, 36, 3) //  Right High Beam Position Request Signal
DECLARE_PACK_UNPACK(ADAS_AHB_DOW, ADAS_DOW_Mode_Feed, 33, 2) //  Door Open Warning Mode select Feedback

struct MHU_ADAS_req_t {
    uint8_t data[8] = {0};
};

DECLARE_PACK_UNPACK(MHU_ADAS_req, CHKSM_MHU_ADAS_req, 7, 8) // Checksum.
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_LA_AudWarning, 15, 2) // Setting Audible warning for LA On off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_RAEB_Mode, 13, 2) // RAEB mode select
DECLARE_PACK_UNPACK(MHU_ADAS_req, ALV_MHU_ADAS_req, 11, 4) // Alive Counter.
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_RCTA_On, 23, 2) // RCTA On off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_BSD_AudWarning, 21, 2) // Setting Audible warning for BSD On off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_ALC_ON, 19, 2) // Turn ALCA on/off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_BSD_On, 17, 2) // BSD On off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_IA_Mode, 31, 2) // IA mode select
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_AHB_On, 29, 2) // AHB On off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_PA_On, 27, 2) // PA On off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_RCTA_sens, 25, 2) // RCTA Sensitivity (Early   Normal   Late)
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_FCW_sens, 39, 2) // FCW Sensitivity
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_AHB_sens, 37, 2) // AHB Sensitivity (Early   Normal   Late)
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_DOW_Mode, 35, 2) // Door Open Warning Mode select
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_TSR_Mode, 33, 2) // TSR Mode selection
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_SET_TSR_SLAert, 47, 2) // TSR Speed Limit Alert select
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_SET_TSR_AudWarn, 45, 2) // TSR Audible Warning On Off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_FCW_Mode, 43, 2) // FCW ABE mode select
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_LA_Mode, 41, 3) // Lane Assistance Mode select
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_ESA_ON, 54, 1) // ESA function on/off
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_DMS_Warning_level, 53, 2) // Warning switch setting from DMS
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_AEB_Mode, 51, 2) // AEB Confirmation
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_LA_sens, 49, 2) // LA Sensitivity (Early   Normal   Late)
DECLARE_PACK_UNPACK(MHU_ADAS_req, MHU_ELKA_ON, 56, 1) // ELKA function ON/OFF

struct ADAS_ELK_t {
    uint8_t data[8] = {0};
};

DECLARE_PACK_UNPACK(ADAS_ELK, CHKSM_ADAS_ELK, 7, 8) // Checksum
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_ELK_check, 15, 2) // Emergency Lane Keeping check
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_ALC_Mode_feed, 13, 2) // ALCA feedback status
DECLARE_PACK_UNPACK(ADAS_ELK, ALV_ADAS_ELK, 11, 4) // Alive Counter
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_HWA_txtpopup, 23, 4) // HWA Text Popups Based on system states- functional cases
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_AHB_ICON_Feed, 19, 3) // HighBeam ICON Feedback Request
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_handsoff_Warning_Mode_2, 16, 2) // Hands off warning level
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_HWA_state, 30, 3) // HWA state
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_AHB_PopUp_Feed, 27, 4) // PopUP Messaging for AHB function
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_ALC_txtpopup, 39, 4) // ALC Text Popups Based on system states- functional cases
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_handsoff_txtpopup, 35, 2) // Hands off warning popup text
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_Mode2_status, 33, 2) // System mode: Mode 1 & 2
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_LA_Display, 47, 4) // Lane assist function and lane display 
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_ALC_state, 43, 3) // ALC state
DECLARE_PACK_UNPACK(ADAS_ELK, ADAS_Lane_Line_Curva_Radius, 55, 16) // Lane line curvature redius (minus for left curve, plus for right curve, radius > 1000m, display as straight curve

struct BCM_HighBeam_PosReq_t {
    uint8_t data[8] = {0};
};

DECLARE_PACK_UNPACK(BCM_HighBeam_PosReq, CHKSM_BCM_HighBeam_PosReq, 7, 8) // Checksum
DECLARE_PACK_UNPACK(BCM_HighBeam_PosReq, BCM_ADAS_AHB_state, 15, 2) // AHB state
DECLARE_PACK_UNPACK(BCM_HighBeam_PosReq, ALIVE_BCM_HighBeam_PosReq, 11, 4) // Alive counter
DECLARE_PACK_UNPACK(BCM_HighBeam_PosReq, BCM_ADAS_AHB_LBPos_feed, 23, 3) // High Beam Position Request Signal(Left Ramp)
DECLARE_PACK_UNPACK(BCM_HighBeam_PosReq, BCM_ADAS_AHB_RBPos_feed, 20, 3) // High Beam Position Request Signal(Right Ramp)
DECLARE_PACK_UNPACK(BCM_HighBeam_PosReq, BCM_AHB_ICON_Feed, 34, 3) // AHB ICON Feedback

void test_can_ADAS_AHB_DOW(CANBus& canif, bool on);
void test_can_BCM_HighBeam_PosReq(CANBus& canif, bool on);