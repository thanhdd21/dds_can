#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <memory>
#include <iostream>

#include "can_buffer.h"
#include "can.h"
#include "dds_manager.h"
#include "dds_subscriber.h"
#include "dds_publisher.h"

class BridgeManager
{
public:
    explicit BridgeManager(const std::string& can_iface)
        : can_worker_(can_iface)
        , running_(false)
    {}

    ~BridgeManager()
    {
        stop();
    }

    bool init()
    {
        // Ensure the DDS participant is created
        DDSManager::instance();

        if (!can_worker_.isValid())
        {
            std::cerr << "[BridgeManager] CAN interface invalid\n";
            return false;
        }

        running_.store(true);
        return true;
    }

    bool isRunning() const
    {
        return running_.load();
    }

    CANBuffer& getTXBuffer()
    {
        return can_tx_buffer_;
    }

    // ----------------------------------------------------------------------
    // DDS SUBSCRIBERS (DDS -> CAN)
    // callback signature = void(const MsgType&, CANBuffer&)
    // ----------------------------------------------------------------------
    template<typename MsgType, typename PubSubType>
    void addSubscriber(
        const std::string& topic_name,
        std::function<void(const MsgType&, CANBuffer&)> callback)
    {
        if (!callback)
        {
            std::cerr << "[BridgeManager] Null subscriber callback for "
                      << topic_name << "\n";
            return;
        }

        auto wrapped_cb = [this, callback](const MsgType& msg) {
            callback(msg, can_tx_buffer_);
        };

        auto sub = std::make_shared<DDSSubscriber<MsgType, PubSubType>>(
            topic_name, wrapped_cb);

        if (!sub->init())
        {
            std::cerr << "[BridgeManager] Subscriber init failed for "
                      << topic_name << "\n";
            return;
        }

        subscribers_.push_back(sub);
    }

    // ----------------------------------------------------------------------
    // DDS PUBLISHERS (CAN -> DDS)
    // convert_cb signature = void(const struct can_frame&, MsgType&)
    // ----------------------------------------------------------------------
    template<typename MsgType, typename PubSubType>
    void addPublisher(
        const std::string& topic_name,
        std::function<void(const struct can_frame&, MsgType&)> convert_cb)
    {
        if (!convert_cb)
        {
            std::cerr << "[BridgeManager] Null publisher convert callback for "
                      << topic_name << "\n";
            return;
        }

        auto pub = std::make_shared<DDSPublisher<MsgType, PubSubType>>(topic_name);

        if (!pub->init())
        {
            std::cerr << "[BridgeManager] Publisher init failed for "
                      << topic_name << "\n";
            return;
        }

        PublisherWrapper w;
        w.owner = pub;
        w.publish_cb = [pub, convert_cb](const struct can_frame& frame) {
            MsgType msg{};
            convert_cb(frame, msg);
            pub->publish(msg);
        };

        publishers_.push_back(std::move(w));
    }

    // ----------------------------------------------------------------------
    // THREADS: CAN RX, CAN TX, DDS publisher
    // ----------------------------------------------------------------------
    void run()
    {
        // TX thread: DDS→CAN
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

        // RX thread: CAN→queue
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

        // DDS publisher thread
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
    // STOP EVERYTHING CLEANLY
    // ----------------------------------------------------------------------
    void stop()
    {
        if (!running_.exchange(false))
            return; // already stopped
        can_tx_buffer_.stop();
        can_worker_.closeSocket();

        // threads
        if (tx_thread_.joinable()) tx_thread_.join();
        if (rx_thread_.joinable()) rx_thread_.join();
        if (dds_pub_thread_.joinable()) dds_pub_thread_.join();

        // subscribers
        for (auto& s : subscribers_)
            if (s) s->shutdown();
        subscribers_.clear();

        // publishers (automatic cleanup via shared_ptr)
        publishers_.clear();
    }

private:
    struct PublisherWrapper
    {
        std::shared_ptr<void> owner;
        std::function<void(const struct can_frame&)> publish_cb;
    };

    CANBus can_worker_;
    CANBuffer can_tx_buffer_;

    std::queue<struct can_frame> can_rx_queue_;
    std::mutex can_rx_mtx_;

    std::vector<std::shared_ptr<DDSBaseSubscriber>> subscribers_;
    std::vector<PublisherWrapper> publishers_;

    std::thread tx_thread_;
    std::thread rx_thread_;
    std::thread dds_pub_thread_;

    std::atomic<bool> running_;
};

