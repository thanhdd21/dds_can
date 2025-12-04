#include "bridge_manager.h"
#include <iostream>
#include <chrono>

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
    // TX thread: DDS → CAN
    tx_thread_ = std::thread([this]() {
        while (running_.load())
        {
            struct can_frame frame{};
            if (can_tx_buffer_.wait_pop(frame))
            {
                if (!can_worker_.send(frame))
                    std::cerr << "[CAN TX] Failed to send frame\n";
            }
        }
    });

    // RX thread: CAN → queue
    rx_thread_ = std::thread([this]() {
        while (running_.load())
        {
            struct can_frame frame{};
            if (can_worker_.receive(frame))
            {
                std::lock_guard<std::mutex> lock(can_rx_mtx_);
                can_rx_queue_.push(frame);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    });

    // DDS publish thread: queue → DDS publishers
    dds_pub_thread_ = std::thread([this]() {
        while (running_.load())
        {
            struct can_frame frame{};

            {
                std::lock_guard<std::mutex> lock(can_rx_mtx_);
                if (can_rx_queue_.empty())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
                frame = can_rx_queue_.front();
                can_rx_queue_.pop();
            }

            for (auto& w : publishers_)
            {
                if (w.publish_cb)
                    w.publish_cb(frame);
            }
        }
    });
}

// ----------------------------------------------------------------------
// STOP EVERYTHING
// ----------------------------------------------------------------------
void BridgeManager::stop()
{
    if (!running_.exchange(false))
        return;  // already stopped

    can_tx_buffer_.stop();
    can_worker_.closeSocket();

    if (tx_thread_.joinable()) tx_thread_.join();
    if (rx_thread_.joinable()) rx_thread_.join();
    if (dds_pub_thread_.joinable()) dds_pub_thread_.join();

    for (auto& s : subscribers_)
        if (s) s->shutdown();
    subscribers_.clear();

    publishers_.clear();
}
