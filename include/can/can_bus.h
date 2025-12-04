#pragma once
#include <linux/can.h>
#include <string>

// struct can_filter {
//     canid_t can_id;   // filter ID
//     canid_t can_mask; // filter mask
// };

class CANBus {
public:
    explicit CANBus(const std::string& interface = "can0");
    ~CANBus();

    bool isValid() const;
    bool send(const struct can_frame& frame);
    bool receive(struct can_frame& frame);
    void closeSocket();
    bool addFilter(struct can_filter* filter, size_t count);

private:
    int sock_;
    std::string ifname_;
    void openSocket();
};
