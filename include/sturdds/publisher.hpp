#ifndef STURDDS_PUBLISHER_HPP
#define STURDDS_PUBLISHER_HPP

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <iostream>
#include <stdexcept>

#include "sturdds/NavMessagePubSubTypes.hpp"

namespace sturdds {

template <typename DataType>
class Publisher : public eprosima::fastdds::dds::DataWriterListener {
 public:
  /**
   * *=== Publisher ===*
   * @brief constructor
   */
  Publisher(
      const std::string& topic_name,
      eprosima::fastdds::dds::TopicDataType* topic_type,
      eprosima::fastdds::dds::DomainParticipant* participant,
      eprosima::fastdds::dds::DataWriterQos qos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT)
      : topic_name_{topic_name},
        participant_{participant},
        publisher_{nullptr},
        writer_{nullptr},
        type_{topic_type},
        topic_{nullptr},
        num_matches_{0},
        is_matched_{false} {
    // save participant
    if (participant_ == nullptr) {
      throw std::runtime_error("DomainParticipant cannot be null.");
    }

    // define custom type
    if (type_.register_type(participant_)) {
      throw std::runtime_error("Error registering type: " + std::string(type_.get_type_name()));
    }
    // auto ret = type_.register_type(participant_);
    // std::cout << "register_type returned " << ret << std::endl;

    // create topic
    topic_ = participant_->create_topic(
        topic_name_, type_.get_type_name(), eprosima::fastdds::dds::TopicQos());
    if (topic_ == nullptr) {
      throw std::runtime_error("Error creating DDS Topic: " + topic_name_);
    }

    // create publisher
    publisher_ = participant_->create_publisher(eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT);
    if (publisher_ == nullptr) {
      participant_->delete_topic(topic_);
      throw std::runtime_error("Error creating DDS Publisher.");
    }

    // create data writer
    writer_ = publisher_->create_datawriter(topic_, qos, this);
    if (writer_ == nullptr) {
      participant_->delete_topic(topic_);
      participant_->delete_publisher(publisher_);
      throw std::runtime_error("Error creating DDS DataWriter for topic: " + topic_name_);
    }
  };

  /**
   * *=== ~Publisher ===*
   * @brief Destructor
   */
  virtual ~Publisher() {
    if (writer_ != nullptr) {
      publisher_->delete_datawriter(writer_);
    }
    if (publisher_ != nullptr) {
      participant_->delete_publisher(publisher_);
    }
    if (topic_ != nullptr) {
      participant_->delete_topic(topic_);
    }
  };

  /**
   * *=== Publish ===*
   * @brief Publish a message
   */
  bool Publish(const DataType& data) {
    if (is_matched_) {
      return writer_->write(&data) == eprosima::fastdds::dds::RETCODE_OK;
    } else {
      return false;
    }
  }

  /**
   * *=== IsMatched ===*
   * @brief query match status
   * @return True if matched with at least one publisher, false otherwise.
   */
  bool IsMatched() const {
    return is_matched_;
  }

  /**
   * *=== GetNumSubscribers ===*
   * @brief query number of subscribers
   * @return The number of matched subscribers.
   */
  int GetNumSubscribers() const {
    return num_matches_;
  }

  /**
   * *=== GetWriter ===*
   * @brief query the data writer
   * @return A pointer to the DataWriter.
   */
  eprosima::fastdds::dds::DataWriter* GetWriter() const {
    return writer_;
  }

 protected:
  std::string topic_name_;
  eprosima::fastdds::dds::DomainParticipant* participant_;
  eprosima::fastdds::dds::Publisher* publisher_;
  eprosima::fastdds::dds::DataWriter* writer_;
  eprosima::fastdds::dds::TypeSupport type_;
  eprosima::fastdds::dds::Topic* topic_;
  int num_matches_;
  bool is_matched_;

  /**
   * *=== on_publication_matched ===*
   * @brief callback for when a subscriber has been matched or unmatched
   * @param reader The DataWriter involved in the event.
   * @param info Information about the matching status change.
   */
  void on_publication_matched(
      eprosima::fastdds::dds::DataWriter* writer,
      const eprosima::fastdds::dds::PublicationMatchedStatus& info) override {
    if (info.current_count_change == 1) {
      std::cout << "Publisher matched a subscriber (Writer: " << writer->guid() << ")."
                << std::endl;
      is_matched_ = true;
      num_matches_++;
      // on_matched(writer);
    } else if (info.current_count_change == -1) {
      std::cout << "Publisher unmatched a subscriber (Writer: " << writer->guid() << ")."
                << std::endl;
      num_matches_--;
      if (num_matches_ == 0) {
        is_matched_ = false;
      }
      // on_unmatched(writer);
    }
  };

  // // Other DataWriterListener callbacks (can be overridden in derived classes)
  // virtual void on_matched(eprosima::fastdds::dds::DataWriter* writer) {
  // }
  // virtual void on_unmatched(eprosima::fastdds::dds::DataWriter* writer) {
  // }
  // void on_offered_deadline_missed(
  //     eprosima::fastdds::dds::DataWriter* writer,
  //     const eprosima::fastdds::dds::OfferedDeadlineMissedStatus& info) override {
  // }
  // void on_offered_incompatible_qos(
  //     eprosima::fastdds::dds::DataWriter* writer,
  //     const eprosima::fastdds::dds::OfferedIncompatibleQosStatus& info) override {
  // }
  // void on_liveliness_lost(
  //     eprosima::fastdds::dds::DataWriter* writer,
  //     const eprosima::fastdds::dds::LivelinessLostStatus& info) override {
  // }
  // void on_reliable_writer_cache_full(
  //     eprosima::fastdds::dds::DataWriter* writer,
  //     const eprosima::fastdds::dds::ReliableWriterCacheFullStatus& info) override {
  // }
  // void on_reliable_writer_change_lost(
  //     eprosima::fastdds::dds::DataWriter* writer,
  //     const eprosima::fastdds::dds::ReliableWriterChangeLostStatus& info) override {
  // }
};

}  // namespace sturdds

#endif