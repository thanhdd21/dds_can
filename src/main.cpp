#include <csignal>
#include <iostream>
#include <thread>
#include <memory>

#include "bridge_manager.h"
#include "dds_callbacks.h"
#include "ahb_can.h"

// std::shared_ptr<BridgeManager> g_bridge;

// void signal_handler(int)
// {
//     if (g_bridge) {
//         std::cout << "[MAIN] SIGINT received, stopping bridge...\n";
//         g_bridge->stop();
//     }
// }

// int main()
// {
//     std::signal(SIGINT, signal_handler);
//     std::cout << "[MAIN] DDS ↔ CAN bridge starting...\n";

//     g_bridge = std::make_shared<BridgeManager>("can0");

//     if (!g_bridge->init()) return -1;

//     g_bridge->addPublisher<soc_monitor::msg::SystemMonitorMsg,
//                            soc_monitor::msg::SystemMonitorMsgPubSubType>(
//         "CanToSystemTopic", canFrameToSystemMonitorMsg);
//     g_bridge->addSubscriber<soc_monitor::msg::SystemMonitorMsg,
//                             soc_monitor::msg::SystemMonitorMsgPubSubType>(
//         "rt/SystemMonitorTopic/SystemMonitor", topicSystemMonitorCallback);
//     g_bridge->run();

//     while (g_bridge->isRunning())
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));

//     std::cout << "[MAIN] Exiting cleanly\n";
//     return 0;
// }

std::atomic<bool> running{false};
std::thread can_timer_thread;

CANBus can("can0");

void signalHandler(int signum)
{
    std::cout << "\nInterrupt signal (" << signum << ") received.\n";
    running.store(false);
}

void start_can_timer()
{
    running.store(true);
    static std::uint8_t count = 0;
    static bool enable = true;

    can_timer_thread = std::thread([]() {
        using namespace std::chrono;

        auto next_tick = steady_clock::now();

        while (running.load())
        {
            // do the CAN send
            if (count % 20 == 0) {
                enable = !enable;
            } 
            test_can_ADAS_AHB_DOW(can, enable);
            // test_can_BCM_HighBeam_PosReq(can, true);

            // schedule next tick (fixed rate: no drift)
            next_tick += 50ms;
            count++;

            std::this_thread::sleep_until(next_tick);
        }
    });
}

void stop_can_timer()
{
    if (can_timer_thread.joinable())
        can_timer_thread.join();
}

int main()
{
    std::signal(SIGINT, signalHandler);
    // add filter
    // struct can_filter filter[2] = {
    //     { .can_id = 0x39d, .can_mask = CAN_SFF_MASK},
    //     { .can_id = 0x352, .can_mask = CAN_SFF_MASK}
    // };
    // can.addFilter(filter, 2);
    start_can_timer();
    while (running.load())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    stop_can_timer();
    std::cout << "CAN timer stopped. Exiting.\n";
    return 0;
}