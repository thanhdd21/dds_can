#include "dds_manager.h"

using namespace eprosima::fastdds::dds; // RETCODE_OK

DDSManager& DDSManager::instance()
{
    static DDSManager instance;
    return instance;
}

DDSManager::DDSManager()
{
    DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;

    participant_ =
        DomainParticipantFactory::get_instance()->create_participant(0, qos);

    if (!participant_) {
        std::cerr << "[DDSManager] ERROR: Failed to create DomainParticipant!\n";
    } else {
        std::cout << "[DDSManager] DomainParticipant created.\n";
    }
}

DDSManager::~DDSManager()
{
    shutdown();
}

DomainParticipant* DDSManager::get_participant()
{
    return participant_;
}

void DDSManager::shutdown()
{
    if (participant_)
    {
        auto* factory = DomainParticipantFactory::get_instance();

        if (factory->delete_participant(participant_) != RETCODE_OK)
        {
            std::cerr << "[DDSManager] ERROR: Failed to delete DomainParticipant!\n";
        }
        else
        {
            std::cout << "[DDSManager] DomainParticipant deleted.\n";
        }

        participant_ = nullptr;
    }
}
