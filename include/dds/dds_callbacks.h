#pragma once

#include <linux/can.h>
#include "can_buffer.h"
#include "EgoVehicleCommandPubSubTypes.hpp"
#include "SystemMonitorPubSubTypes.hpp"

void topicEgoVehicleCommandCallback(
    const control_msgs::msg::EgoVehicleCommand& msg,
    CANBuffer& can_buffer);

// void topicSystemMonitorCallback(
//     const soc_monitor::msg::SystemMonitorMsg& msg,
//     CANBuffer& can_buffer);

// void topicThermalZoneCallback(
//     const soc_monitor::msg::ThermalZone& msg,
//     CANBuffer& can_buffer);

// void canFrameToSystemMonitorMsg(
//     const struct can_frame& frame,
//     soc_monitor::msg::SystemMonitorMsg& msg);


