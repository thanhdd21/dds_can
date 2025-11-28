#include <csignal>
#include <iostream>
#include <thread>
#include <memory>

#include "bridge_manager.h"
#include "dds_callbacks.h"

std::shared_ptr<BridgeManager> g_bridge;

void signal_handler(int)
{
    if (g_bridge) {
        std::cout << "[MAIN] SIGINT received, stopping bridge...\n";
        g_bridge->stop();
    }
}

int main()
{
    std::signal(SIGINT, signal_handler);
    std::cout << "[MAIN] DDS ↔ CAN bridge starting...\n";

    g_bridge = std::make_shared<BridgeManager>("can0");

    if (!g_bridge->init()) return -1;

    g_bridge->addPublisher<soc_monitor::msg::SystemMonitorMsg,
                           soc_monitor::msg::SystemMonitorMsgPubSubType>(
        "CanToSystemTopic", canFrameToSystemMonitorMsg);
    g_bridge->addSubscriber<soc_monitor::msg::SystemMonitorMsg,
                            soc_monitor::msg::SystemMonitorMsgPubSubType>(
        "rt/SystemMonitorTopic/SystemMonitor", topicSystemMonitorCallback);
    g_bridge->run();

    while (g_bridge->isRunning())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "[MAIN] Exiting cleanly\n";
    return 0;
}
