#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <linux/can.h>
#include <atomic>

class CANBuffer
{
public:
    void push(const struct can_frame& frame)
    {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(frame);
        }
        cv_.notify_one();
    }

    bool pop(can_frame& frame) {
        std::lock_guard<std::mutex> lock(mtx_);
        if(queue_.empty()) return false;
        frame = queue_.front();
        queue_.pop();
        return true;
    }

    bool wait_pop(struct can_frame& frame)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]{ return stopped_ || !queue_.empty(); });
        // if (stopped_ && queue_.empty())
        //     return false;
        if (stopped_) // immediately
            return false;
        frame = queue_.front();
        queue_.pop();
        return true;
    }

    void stop() {
        stopped_ = true;
        cv_.notify_all();
    }

private:
    std::queue<struct can_frame> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> stopped_{false};
};
