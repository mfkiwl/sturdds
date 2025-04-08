#ifndef STURDDS_SUBSCRIBER_HPP
#define STURDDS_SUBSCRIBER_HPP

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <functional>
#include <iostream>

namespace sturdds {

template <typename DataType>
class Subscriber : public eprosima::fastdds::dds::DataReaderListener {
 public:
  /**
   * *=== Subscriber ===*
   * @brief Constructor
   */
  Subscriber(
      const std::string& topic_name,
      eprosima::fastdds::dds::TopicDataType* topic_type,
      std::function<void(const DataType&)> callback_func,
      eprosima::fastdds::dds::DomainParticipant* participant,
      eprosima::fastdds::dds::DataReaderQos qos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT)
      : topic_name_{topic_name},
        participant_{participant},
        subscriber_{nullptr},
        reader_{nullptr},
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

    // create subscriber
    subscriber_ =
        participant_->create_subscriber(eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT, nullptr);
    if (subscriber_ == nullptr) {
      participant_->delete_topic(topic_);
      throw std::runtime_error("Error creating DDS Subscriber.");
    }

    // create data reader
    reader_ = subscriber_->create_datareader(topic_, qos, this);
    if (reader_ == nullptr) {
      participant_->delete_topic(topic_);
      participant_->delete_subscriber(subscriber_);
      throw std::runtime_error("Error creating DDS DataReader for topic: " + topic_name_);
    }

    // define process_data_ function
    process_data_ = callback_func;
  };

  /**
   * *=== ~Subscriber ===*
   * @brief Destructor
   */
  virtual ~Subscriber() {
    if (reader_ != nullptr) {
      subscriber_->delete_datareader(reader_);
    }
    if (subscriber_ != nullptr) {
      participant_->delete_subscriber(subscriber_);
    }
    if (topic_ != nullptr) {
      participant_->delete_topic(topic_);
    }
  };

  /**
   * *=== IsMatched ===*
   * @brief Query the match status
   * @return True if matched with at least one publisher, false otherwise.
   */
  bool IsMatched() const {
    return is_matched_;
  }

  /**
   * *=== GetNumPublishers ===*
   * @brief Get the number of currently matched publishers.
   * @return The number of matched publishers.
   */
  int GetNumPublishers() const {
    return num_matches_;
  }

  /**
   * *=== GetReader ===*
   * @brief Get the underlying DataReader.
   * @return A pointer to the DataReader.
   */
  eprosima::fastdds::dds::DataReader* GetReader() const {
    return reader_;
  }

 protected:
  std::string topic_name_;
  eprosima::fastdds::dds::DomainParticipant* participant_;
  eprosima::fastdds::dds::Subscriber* subscriber_;
  eprosima::fastdds::dds::DataReader* reader_;
  eprosima::fastdds::dds::TypeSupport type_;
  eprosima::fastdds::dds::Topic* topic_;
  int num_matches_;
  bool is_matched_;
  std::function<void(const DataType&)> process_data_;

  /**
   * *=== on_data_available ===*
   * @brief Callback executed when data is available for the DataReader.
   * @param reader The DataReader with available data.
   */
  void on_data_available(eprosima::fastdds::dds::DataReader* reader) override {
    eprosima::fastdds::dds::SampleInfo info;
    DataType data;
    if (reader->take_next_sample(&data, &info) == eprosima::fastdds::dds::RETCODE_OK) {
      if (info.valid_data) {
        process_data_(data);  // Call virtual method to process the received data
      }
    }
  };

  /**
   * *=== on_subscription_matched ===*
   * @brief Callback executed when a publisher is matched or unmatched.
   * @param reader The DataReader involved in the event.
   * @param info Information about the matching status change.
   */
  void on_subscription_matched(
      eprosima::fastdds::dds::DataReader* reader,
      const eprosima::fastdds::dds::SubscriptionMatchedStatus& info) override {
    if (info.current_count_change == 1) {
      std::cout << "Subscriber matched a publisher (Reader: " << reader->guid() << ")."
                << std::endl;
      is_matched_ = true;
      num_matches_++;
      // on_matched(reader);  // Call virtual hook for derived classes
    } else if (info.current_count_change == -1) {
      std::cout << "Subscriber unmatched a publisher (Reader: " << reader->guid() << ")."
                << std::endl;
      num_matches_--;
      if (num_matches_ == 0) {
        is_matched_ = false;
      }
      // on_unmatched(reader);  // Call virtual hook for derived classes
    }
  };

  // // Other DataReaderListener callbacks (can be overridden in derived classes)
  // virtual void on_matched(eprosima::fastdds::dds::DataReader* reader) {
  // }
  // virtual void on_unmatched(eprosima::fastdds::dds::DataReader* reader) {
  // }
  // virtual void on_requested_deadline_missed(
  //     eprosima::fastdds::dds::DataReader* reader,
  //     const eprosima::fastdds::dds::RequestedDeadlineMissedStatus& info) override {
  // }
  // virtual void on_requested_incompatible_qos(
  //     eprosima::fastdds::dds::DataReader* reader,
  //     const eprosima::fastdds::dds::RequestedIncompatibleQosStatus& info) override {
  // }
  // virtual void on_sample_rejected(
  //     eprosima::fastdds::dds::DataReader* reader,
  //     const eprosima::fastdds::dds::SampleRejectedStatus& info) override {
  // }
  // virtual void on_liveliness_changed(
  //     eprosima::fastdds::dds::DataReader* reader,
  //     const eprosima::fastdds::dds::LivelinessChangedStatus& info) override {
  // }
  // virtual void on_data_reader_cache_full(
  //     eprosima::fastdds::dds::DataReader* reader,
  //     const eprosima::fastdds::dds::DataReaderCacheFullStatus& info) override {
  // }
  // virtual void on_reliable_reader_cache_changed(
  //     eprosima::fastdds::dds::DataReader* reader,
  //     const eprosima::fastdds::dds::ReliableReaderCacheChangedStatus& info) override {
  // }
};

}  // namespace sturdds

#endif