#pragma once
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "dds_manager.h"
#include <functional>
#include <iostream>

class DDSBaseSubscriber
{
public:
    virtual ~DDSBaseSubscriber() {}
    virtual void shutdown() = 0;
};

template<typename MsgType, typename PubSubType>
class DDSSubscriber :
    public DDSBaseSubscriber,
    public eprosima::fastdds::dds::DataReaderListener
{
public:
    DDSSubscriber(const std::string& topic_name,
                  std::function<void(const MsgType&)> callback)
        : topic_name_(topic_name), callback_(callback)
    {}

    ~DDSSubscriber() { shutdown(); }

    bool init()
    {
        using namespace eprosima::fastdds::dds;

        participant_ = DDSManager::instance().get_participant();
        if (!participant_) return false;

        type_ = TypeSupport(new PubSubType());
        type_.register_type(participant_);

        topic_ = participant_->create_topic(topic_name_, type_.get_type_name(), TOPIC_QOS_DEFAULT);
        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

	    DataReaderQos qos = DATAREADER_QOS_DEFAULT;
        qos.reliability().kind = RELIABLE_RELIABILITY_QOS;
	    qos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;

        reader_ = subscriber_->create_datareader(topic_, qos, this);
        // reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, this);


        return reader_ != nullptr;
    }

    void on_data_available(eprosima::fastdds::dds::DataReader* reader) override
    {
        using namespace eprosima::fastdds::dds;

        MsgType msg;
        SampleInfo info;

        while (reader->take_next_sample(&msg, &info) == RETCODE_OK)
        {
            if (info.valid_data && callback_) callback_(msg);
        }
    }

    void shutdown() override
    {
        if (subscriber_ && reader_) subscriber_->delete_datareader(reader_);
        if (participant_ && subscriber_) participant_->delete_subscriber(subscriber_);
        if (participant_ && topic_) participant_->delete_topic(topic_);

        reader_ = nullptr;
        subscriber_ = nullptr;
        topic_ = nullptr;
    }

private:
    std::string topic_name_;
    eprosima::fastdds::dds::TypeSupport type_;

    eprosima::fastdds::dds::DomainParticipant* participant_{nullptr};
    eprosima::fastdds::dds::Subscriber* subscriber_{nullptr};
    eprosima::fastdds::dds::DataReader* reader_{nullptr};
    eprosima::fastdds::dds::Topic* topic_{nullptr};

    std::function<void(const MsgType&)> callback_;
};
