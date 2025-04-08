#ifndef STURDDS_INTERFACE_HPP
#define STURDDS_INTERFACE_HPP

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <functional>
#include <memory>

#include "sturdds/publisher.hpp"
#include "sturdds/subscriber.hpp"

namespace sturdds {

class Interface {
 public:
  /**
   * *=== Interface ===*
   * @brief Constructor
   */
  Interface(
      int domain_id = 0,
      const eprosima::fastdds::dds::DomainParticipantQos& qos =
          eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);

  /**
   * *=== ~Interface
   * @brief Destructor
   */
  ~Interface();

  /**
   * *=== GetParticipant ===*
   * @brief Query the underlying participant
   * @return pointer to the DomainParticipant
   */
  eprosima::fastdds::dds::DomainParticipant* GetParticipant() const;

  /**
   * *=== GetDomainId ===*
   * @brief Query the domain id
   * @return The domain id
   */
  int GetDomainId() const;

  /**
   * *=== CreatePublisher ===*
   * @brief Create a DDS publisher with requested topic
   * @param
   */
  template <typename DataType>
  std::unique_ptr<Publisher<DataType>> CreatePublisher(
      const std::string topic_name,
      eprosima::fastdds::dds::TopicDataType* topic_type,
      eprosima::fastdds::dds::DataWriterQos qos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT) {
    return std::make_unique<Publisher<DataType>>(topic_name, topic_type, participant_, qos);
  }

  template <typename DataType>
  std::unique_ptr<Subscriber<DataType>> CreateSubscriber(
      const std::string topic_name,
      eprosima::fastdds::dds::TopicDataType* topic_type,
      std::function<void(const DataType&)> callback_func,
      eprosima::fastdds::dds::DataReaderQos qos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT) {
    return std::make_unique<Subscriber<DataType>>(
        topic_name, topic_type, callback_func, participant_, qos);
  }

 private:
  int domain_id_;
  eprosima::fastdds::dds::DomainParticipant* participant_;
  eprosima::fastdds::dds::DomainParticipantQos qos_;
};

}  // namespace sturdds

#endif