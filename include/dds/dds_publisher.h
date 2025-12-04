#pragma once
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#include "dds_manager.h"
#include <iostream>

template<typename MsgType, typename PubSubType>
class DDSPublisher
{
public:
    explicit DDSPublisher(const std::string& topic_name)
        : topic_name_(topic_name)
    {}

    ~DDSPublisher() { shutdown(); }

    bool init()
    {
        using namespace eprosima::fastdds::dds;

        participant_ = DDSManager::instance().get_participant();
        if (!participant_) return false;

        type_ = TypeSupport(new PubSubType());
        type_.register_type(participant_);

        topic_ = participant_->create_topic(
            topic_name_,
            type_.get_type_name(),
            TOPIC_QOS_DEFAULT);

        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (!publisher_) return false;

        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT);
        return writer_ != nullptr;
    }

    bool publish(const MsgType& msg)
    {
        using namespace eprosima::fastdds::dds;
        return writer_ &&
            writer_->write(const_cast<MsgType*>(&msg)) == RETCODE_OK;
    }

    void shutdown()
    {
        if (publisher_ && writer_) publisher_->delete_datawriter(writer_);
        if (participant_ && publisher_) participant_->delete_publisher(publisher_);
        if (participant_ && topic_) participant_->delete_topic(topic_);

        writer_ = nullptr;
        publisher_ = nullptr;
        topic_ = nullptr;
    }

private:
    std::string topic_name_;
    eprosima::fastdds::dds::TypeSupport type_;

    eprosima::fastdds::dds::DomainParticipant* participant_{nullptr};
    eprosima::fastdds::dds::Publisher* publisher_{nullptr};
    eprosima::fastdds::dds::DataWriter* writer_{nullptr};
    eprosima::fastdds::dds::Topic* topic_{nullptr};
};
