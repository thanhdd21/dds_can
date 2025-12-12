#include "can_bus.h"
#include "can_msgs.h"
#include "crc8_j1850.h"
#include <linux/can.h>
#include <iostream>

std::uint8_t ComAppl_GenAc_Std(std::uint8_t lastAliveCounter)
{
    std::uint8_t msgCtr_u8 = lastAliveCounter;

    if(msgCtr_u8 < 14)
    {
        msgCtr_u8++;
    }
    else
    {
        msgCtr_u8 = 0;
    }

    lastAliveCounter = msgCtr_u8;

    return (msgCtr_u8);
}

void test_can_ADAS_AHB_DOW(CANBus& canif, bool on)
{
    ADAS_AHB_DOW_t msg;
    static std::uint8_t aliveCounter = 0;
    std::uint8_t checksum = 0;
    struct can_frame frame{};
    struct can_frame read_frame{};

    if (!canif.isValid())
    {
        std::cerr << "CAN interface invalid\n";
        return;
    }

    if (canif.receive(frame)) {
        // std::cout << "can read1 " << std::hex << static_cast<int>(frame.can_id) << " " << static_cast<int>(frame.data[1]) << "\n";
        if (frame.can_id == CAN_MSG_ID(ADAS_AHB_DOW)) {
            aliveCounter = static_cast<int>(frame.data[1]) & 0xF;
            std::cout << "aliveCounter1: " << static_cast<int>(aliveCounter) << "\n";
        }
    }
    aliveCounter = ComAppl_GenAc_Std(aliveCounter);
    pack_ADAS_AHB_DOW_ALV_ADAS_AHB_DOW(msg, aliveCounter);
    if (on) {
        pack_ADAS_AHB_DOW_ADAS_AHB_state(msg, 2);
        pack_ADAS_AHB_DOW_ADAS_AHB_check(msg, 0);
        pack_ADAS_AHB_DOW_ADAS_AHB_LBPos(msg, 1);
        pack_ADAS_AHB_DOW_ADAS_AHB_RBPos(msg, 1);
        pack_ADAS_AHB_DOW_ADAS_AHB_Mode_Feed(msg, 1);
    }

    checksum = crc::crc8_j1850(&msg.data[1], 7);
    pack_ADAS_AHB_DOW_CHKSM_ADAS_AHB_DOW(msg, checksum);

    frame.can_id = MSG_ADAS_AHB_DOW_ID;
    frame.can_dlc = 8;
    for (size_t i = 0; i < 8; ++i) {
        frame.data[i] = msg.data[i];
    }
    // frame.data[7] = aliveCounter;
    if (!canif.send(frame))
        std::cerr << "Failed to send frame\n";
}

void test_can_BCM_HighBeam_PosReq(CANBus& canif, bool on)
{
    BCM_HighBeam_PosReq_t msg;
    static std::uint8_t aliveCounter = 0;
    std::uint8_t checksum = 0;
    struct can_frame frame{};

    if (!canif.isValid())
    {
        std::cerr << "CAN interface invalid\n";
        return;
    }
    if (canif.receive(frame)) {
        // std::cout << "can read2 " << std::hex << static_cast<int>(frame.can_id) << " " << static_cast<int>(frame.data[1]) << "\n";
        if (frame.can_id == 0x39d) {
            aliveCounter = static_cast<int>(frame.data[1]) & 0xF;
            std::cout << "aliveCounter2: " << static_cast<int>(aliveCounter) << "\n";
        }
    }
    aliveCounter = ComAppl_GenAc_Std(aliveCounter);
    pack_BCM_HighBeam_PosReq_ALIVE_BCM_HighBeam_PosReq(msg, aliveCounter);
    if (on) {
        pack_BCM_HighBeam_PosReq_BCM_ADAS_AHB_state(msg, 2);
        pack_BCM_HighBeam_PosReq_BCM_ADAS_AHB_LBPos_feed(msg, 1);
        pack_BCM_HighBeam_PosReq_BCM_ADAS_AHB_RBPos_feed(msg, 1);
        pack_BCM_HighBeam_PosReq_BCM_AHB_ICON_Feed(msg, 3);
    }

    checksum = crc::crc8_j1850(&msg.data[1], 7);
    pack_BCM_HighBeam_PosReq_CHKSM_BCM_HighBeam_PosReq(msg, checksum);

    frame.can_id = CAN_MSG_ID(BCM_HighBeam_PosReq);
    frame.can_dlc = 8;
    for (size_t i = 0; i < 8; ++i) {
        frame.data[i] = msg.data[i];
    }
    if (!canif.send(frame))
        std::cerr << "Failed to send frame\n";
}