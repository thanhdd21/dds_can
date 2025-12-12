#include "dds_topic.h"

void EgoVehicleCommand::set_high_beam(std::uint8_t v)
{
    std::lock_guard<std::mutex> lock(mtx_);
    high_beam_ = v;
}

std::uint8_t EgoVehicleCommand::get_high_beam() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return high_beam_;
}

EgoVehicleCommand& ego_vehicle_command()
{
    static EgoVehicleCommand instance;
    return instance;
}