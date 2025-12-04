#pragma once
#include <cstdint>

// ------------------------------
// Enums for signal values
// ------------------------------

enum class ADAS_AHB_State : uint8_t
{
    OFF = 0,
    HighBeam_Passive = 1,
    HighBeam_Active = 2,
    Fault = 3
};

enum class ADAS_AHB_Check : uint8_t
{
    Available = 0,
    Temporarily_NotAvailable = 1,
    Permanently_NotAvailable = 2
};

enum class ADAS_AHB_BeamPos : uint8_t
{
    OffLowBeam = 0,
    HighBeam = 1,
    Reserved = 2 // 2-7 reserved/SNA
};

enum class ADAS_AHB_Mode_Feed : uint8_t
{
    Off = 0,
    On = 1
};

enum class ADAS_AHB_Sens_Feed : uint8_t
{
    Early = 0,
    Normal = 1,
    Late = 2,
    Reserved = 3
};

enum class MHU_AHB_On : uint8_t
{
    OFF = 0,
    ON = 1,
    Reserved = 2,
    Signal_Invalid = 3
};

enum class MHU_AHB_Sens : uint8_t
{
    Early = 0,
    Normal = 1,
    Late = 2,
    Signal_Invalid = 3
};

enum class ADAS_AHB_ICON_Feed : uint8_t
{
    Off = 0,
    Gray = 1,
    Green = 2,
    White = 3,
    Yellow = 4,
    Red = 5
};

enum class ADAS_AHB_PopUp_Feed : uint8_t
{
    Off = 0,
    PopUp1 = 1,
    PopUp2 = 2,
    PopUp3 = 3,
    PopUp4 = 4,
    PopUp5 = 5,
    PopUp6 = 6,
    PopUp7 = 7,
    PopUp8 = 8,
    PopUp9 = 9,
    PopUp10 = 10,
    Reserved11 = 11,
    Reserved12 = 12,
    Reserved13 = 13,
    Reserved14 = 14,
    Invalid = 15
};

enum class BCM_ADAS_AHB_State : uint8_t
{
    OFF = 0,
    Passive = 1,
    Active = 2,
    Fault = 3
};

enum class BCM_ADAS_AHB_BeamPos_Feed : uint8_t
{
    OffLowBeam = 0,
    HighBeam = 1,
    Reserved = 2 // 2-7 reserved
};

enum class BCM_AHB_ICON_Feed : uint8_t
{
    Off = 0,
    Gray = 1,
    Green = 2,
    White = 3,
    Yellow = 4,
    Red = 5
};

// ------------------------------
// CAN message structs
// ------------------------------

struct ADAS_AHB_DOW
{
    ADAS_AHB_State ahb_state;
    ADAS_AHB_Check ahb_check;
    ADAS_AHB_BeamPos lb_pos;
    ADAS_AHB_BeamPos rb_pos;
    ADAS_AHB_Mode_Feed mode_feed;
    ADAS_AHB_Sens_Feed sens_feed;
};

struct MHU_ADAS_req
{
    MHU_AHB_On ahb_on;
    MHU_AHB_Sens ahb_sens;
};

struct ADAS_ELK
{
    ADAS_AHB_ICON_Feed icon_feed;
    ADAS_AHB_PopUp_Feed popup_feed;
};

struct BCM_HighBeam_PosReq
{
    BCM_ADAS_AHB_State ahb_state;
    BCM_ADAS_AHB_BeamPos_Feed lb_pos_feed;
    BCM_ADAS_AHB_BeamPos_Feed rb_pos_feed;
    BCM_AHB_ICON_Feed icon_feed;
};