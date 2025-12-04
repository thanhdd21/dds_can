#pragma once
#include "SystemMonitorPubSubTypes.hpp"
#include "can_buffer.h"
#include <linux/can.h>
#include <iostream>

// DDS -> CAN callbacks
inline void topicSystemMonitorCallback(const soc_monitor::msg::SystemMonitorMsg& msg, CANBuffer& can_buffer)
{
    struct can_frame frame{};
    frame.can_id = 0x100;
    frame.can_dlc = 2;

    uint16_t cpu = static_cast<uint16_t>(msg.avg_cpu_load() * 100);
    frame.data[0] = (cpu >> 8) & 0xFF;
    frame.data[1] = cpu & 0xFF;

    can_buffer.push(frame);

     // --- Print full message ---
    std::cout << "[DDS->CAN] SystemMonitorMsg:\n";

    // Thermal zones
    const auto& thermals = msg.thermals();
    std::cout << "  Thermal Zones:\n";
    for (size_t i = 0; i < thermals.size(); ++i) {
        const auto& t = thermals[i];
        std::cout << "    " << t.name() << ": " << t.temperature() << "°C\n";
    }

    // CPU core loads
    const auto& cpu_loads = msg.cpu_core_loads();
    std::cout << "  CPU Core Loads:\n";
    for (size_t i = 0; i < cpu_loads.size(); ++i) {
        std::cout << "    Core " << i << ": " << cpu_loads[i] << "%\n";
    }

    // Average, max CPU load and RAM usage
    std::cout << "  Avg CPU Load: " << msg.avg_cpu_load() << "%\n";
    std::cout << "  Max CPU Load: " << msg.max_cpu_load() << "%\n";
    std::cout << "  RAM Usage:    " << msg.ram_usage() << " MB\n";

    std::cout << "[DDS->CAN] SystemMonitorMsg pushed\n";
}

inline void topicThermalZoneCallback(const soc_monitor::msg::ThermalZone& msg, CANBuffer& can_buffer)
{
    struct can_frame frame{};
    frame.can_id = 0x101;
    frame.can_dlc = 2;

    uint16_t temp = static_cast<uint16_t>(msg.temperature() * 100);
    frame.data[0] = (temp >> 8) & 0xFF;
    frame.data[1] = temp & 0xFF;

    can_buffer.push(frame);
    std::cout << "[DDS->CAN] ThermalZone pushed\n";
}

// CAN -> DDS conversion
inline void canFrameToSystemMonitorMsg(const struct can_frame& frame,
                                       soc_monitor::msg::SystemMonitorMsg& msg)
{
    msg.avg_cpu_load(static_cast<float>((frame.data[0] << 8) | frame.data[1]) / 100.0f);
    msg.max_cpu_load(msg.avg_cpu_load() + 10.0f);
    msg.ram_usage(50.0f);
}
