#pragma once
#include <linux/can.h>
#include <string>

class CANBus {
public:
    explicit CANBus(const std::string& interface = "can0");
    ~CANBus();

    bool isValid() const;
    bool send(const struct can_frame& frame);
    bool receive(struct can_frame& frame);

private:
    int sock_;
    std::string ifname_;
    void openSocket();
};
