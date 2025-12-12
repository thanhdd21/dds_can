#include <iostream>
#include <chrono>
#include "bridge_manager.h"
#include "can_msgs.h"
#include "dds_topic.h"

extern std::mutex EgoVehicleCommandCallback_mtx;
extern std::uint8_t g_high_beam_light_cmd;
// ----------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------
BridgeManager::BridgeManager(const std::string& can_iface)
    : can_worker_(can_iface)
    , running_(false)
{
}

BridgeManager::~BridgeManager()
{
    stop();
}

// ----------------------------------------------------------------------
// INIT
// ----------------------------------------------------------------------
bool BridgeManager::init()
{
    DDSManager::instance();  // ensure participant exists

    if (!can_worker_.isValid())
    {
        std::cerr << "[BridgeManager] CAN interface invalid\n";
        return false;
    }

    running_.store(true);
    return true;
}

bool BridgeManager::isRunning() const
{
    return running_.load();
}

CANBuffer& BridgeManager::getTXBuffer()
{
    return can_tx_buffer_;
}

// ----------------------------------------------------------------------
// RUN THREADS
// ----------------------------------------------------------------------
void BridgeManager::run()
{
    can_50ms_thread_ = std::thread([this]() {
        using namespace std::chrono;
        auto next_tick = steady_clock::now();
        thread_local struct can_frame ahb_frame{};
        std::uint8_t high_beam_cmd;

        while (running_.load())
        {
            high_beam_cmd = ego_vehicle_command().get_high_beam();
            if (high_beam_cmd) {
                test_can_ADAS_AHB_DOW(can_worker_, true);
                // std::cout <<"sending AHB: true\n";
            } else {
                test_can_ADAS_AHB_DOW(can_worker_, false);
                // std::cout <<"sending AHB: false\n";
            }

            next_tick += 50ms;
            std::this_thread::sleep_until(next_tick);
        }
    });

    // // TX thread: DDS → CAN
    // tx_thread_ = std::thread([this]() {
    //     while (running_.load())
    //     {
    //         struct can_frame frame{};
    //         if (can_tx_buffer_.wait_pop(frame))
    //         {
    //             if (!can_worker_.send(frame))
    //                 std::cerr << "[CAN TX] Failed to send frame\n";
    //         }
    //     }
    // });

    // // RX thread: CAN → queue
    // rx_thread_ = std::thread([this]() {
    //     while (running_.load())
    //     {
    //         struct can_frame frame{};
    //         if (can_worker_.receive(frame))
    //         {
    //             std::lock_guard<std::mutex> lock(can_rx_mtx_);
    //             can_rx_queue_.push(frame);
    //         }
    //         else
    //         {
    //             std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //         }
    //     }
    // });

    // // DDS publish thread: queue → DDS publishers
    // dds_pub_thread_ = std::thread([this]() {
    //     while (running_.load())
    //     {
    //         struct can_frame frame{};

    //         {
    //             std::lock_guard<std::mutex> lock(can_rx_mtx_);
    //             if (can_rx_queue_.empty())
    //             {
    //                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //                 continue;
    //             }
    //             frame = can_rx_queue_.front();
    //             can_rx_queue_.pop();
    //         }

    //         for (auto& w : publishers_)
    //         {
    //             if (w.publish_cb)
    //                 w.publish_cb(frame);
    //         }
    //     }
    // });
}

// ----------------------------------------------------------------------
// STOP EVERYTHING
// ----------------------------------------------------------------------
void BridgeManager::stop()
{
    if (!running_.exchange(false))
        return;  // already stopped

    // can_tx_buffer_.stop();
    can_worker_.closeSocket();

    if (can_50ms_thread_.joinable()) can_50ms_thread_.join();
    // if (tx_thread_.joinable()) tx_thread_.join();
    // if (rx_thread_.joinable()) rx_thread_.join();
    // if (dds_pub_thread_.joinable()) dds_pub_thread_.join();

    for (auto& s : subscribers_)
        if (s) s->shutdown();
    subscribers_.clear();

    publishers_.clear();
}

bool BridgeManager::can_add_filter(struct can_filter* filter, size_t count)
{
    return can_worker_.addFilter(filter, count);
}
