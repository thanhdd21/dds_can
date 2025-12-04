#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <memory>
#include <string>

#include "can_buffer.h"
#include "can_bus.h"
#include "dds_manager.h"
#include "dds_subscriber.h"
#include "dds_publisher.h"

class BridgeManager
{
public:
    explicit BridgeManager(const std::string& can_iface);
    ~BridgeManager();

    bool init();
    bool isRunning() const;

    CANBuffer& getTXBuffer();

    // ----------------------------------------------------------------------
    // DDS SUBSCRIBERS (DDS → CAN)
    // callback: void(const MsgType&, CANBuffer&)
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

        auto sub =
            std::make_shared<DDSSubscriber<MsgType, PubSubType>>(topic_name, wrapped_cb);

        if (!sub->init())
        {
            std::cerr << "[BridgeManager] Subscriber init failed for "
                      << topic_name << "\n";
            return;
        }

        subscribers_.push_back(sub);
    }

    // ----------------------------------------------------------------------
    // DDS PUBLISHERS (CAN → DDS)
    // convert_cb: void(const can_frame&, MsgType&)
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

        PublisherWrapper wrapper;
        wrapper.owner = pub;
        wrapper.publish_cb = [pub, convert_cb](const struct can_frame& frame) {
            MsgType msg{};
            convert_cb(frame, msg);
            pub->publish(msg);
        };

        publishers_.push_back(std::move(wrapper));
    }

    // ----------------------------------------------------------------------
    void run();
    void stop();

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

