#include "can_bus.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <errno.h>

CANBus::CANBus(const std::string& interface)
    : sock_(-1), ifname_(interface)
{
    openSocket();
}

CANBus::~CANBus() {
    if (sock_ >= 0)
        close(sock_);
}

bool CANBus::isValid() const {
    return sock_ >= 0;
}

void CANBus::openSocket() {
    sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock_ < 0) {
        perror("socket");
        return;
    }

    // Make socket non-blocking
    int flags = fcntl(sock_, F_GETFL, 0);
    fcntl(sock_, F_SETFL, flags | O_NONBLOCK);

    struct ifreq ifr{};
    strncpy(ifr.ifr_name, ifname_.c_str(), IFNAMSIZ - 1);

    if (ioctl(sock_, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        close(sock_);
        sock_ = -1;
        return;
    }

    struct sockaddr_can addr{};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock_);
        sock_ = -1;
        return;
    }

    std::cout << "[CAN] Interface " << ifname_ << " initialized\n";
}

bool CANBus::send(const struct can_frame& frame) {
    if (sock_ < 0) return false;

    ssize_t n = write(sock_, &frame, sizeof(frame));
    return n == sizeof(frame);
}

bool CANBus::receive(struct can_frame& frame) {
    if (sock_ < 0) return false;

    ssize_t n = read(sock_, &frame, sizeof(frame));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // no data available
            return false;
        } else {
            perror("CAN read");
            return false;
        }
    }
    return n == sizeof(frame);
}

void CANBus::closeSocket() {
    if (sock_ >= 0) {
        close(sock_);
        sock_ = -1;
    }
}

bool CANBus::addFilter(struct can_filter* filter, size_t count) {
    if (sock_ < 0) return false;
    return setsockopt(sock_, SOL_CAN_RAW, CAN_RAW_FILTER, filter, count * sizeof(struct can_filter)) == 0;
}