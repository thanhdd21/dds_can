#pragma once

#include <cstdint>
#include <mutex>

class EgoVehicleCommand
{
public:
    void set_high_beam(std::uint8_t v);

    std::uint8_t get_high_beam() const;

private:
    mutable std::mutex mtx_;
    std::uint8_t high_beam_{0};
};

EgoVehicleCommand& ego_vehicle_command();