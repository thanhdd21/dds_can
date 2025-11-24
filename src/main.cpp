#include "can.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    CANBus can("can0");

    if (!can.isValid()) {
        std::cerr << "Failed to open CAN interface\n";
        return -1;
    }

    // Send a test CAN message
    struct can_frame frame{};
    frame.can_id = 0x123;
    frame.can_dlc = 4;
    frame.data[0] = 0x11;
    frame.data[1] = 0x22;
    frame.data[2] = 0x33;
    frame.data[3] = 0x44;

    if (can.send(frame)) {
        std::cout << "Sent CAN frame ID=0x123\n";
    } else {
        std::cerr << "Failed to send CAN frame\n";
    }

    // Receive messages in loop
    std::cout << "Listening for CAN messages...\n";
    while (true) {
        struct can_frame recv_frame{};
        if (can.receive(recv_frame)) {
            std::cout << "Received CAN ID=0x" 
                      << std::hex << recv_frame.can_id 
                      << " DLC=" << std::dec << (int)recv_frame.can_dlc 
                      << " Data=";
            for (int i = 0; i < recv_frame.can_dlc; i++)
                std::cout << std::hex << (int)recv_frame.data[i] << " ";
            std::cout << std::dec << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
