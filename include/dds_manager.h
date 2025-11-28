#pragma once

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <iostream>

class DDSManager
{
public:
    static DDSManager& instance();
    bool init();
    eprosima::fastdds::dds::DomainParticipant* get_participant();

    // Explicit shutdown
    void shutdown();

private:
    DDSManager();
    ~DDSManager();

    DDSManager(const DDSManager&) = delete;
    DDSManager& operator=(const DDSManager&) = delete;

    eprosima::fastdds::dds::DomainParticipant* participant_ = nullptr;
};
