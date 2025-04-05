#include "sturdds/interface.hpp"

#include <stdexcept>

namespace sturdds {

// *=== Interface ===*
Interface::Interface(int domain_id, const eprosima::fastdds::dds::DomainParticipantQos& qos)
    : domain_id_{domain_id}, participant_{nullptr}, qos_{qos} {
  participant_ =
      eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
          domain_id_, qos_);
  if (!participant_) {
    throw std::runtime_error(
        "Failed to create DomainParticipant in domain: " + std::to_string(domain_id_));
  }
}

// *=== ~Interface ===*
Interface::~Interface() {
  if (participant_) {
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(
        participant_);
  }
}

// *=== GetParticipant ===*
eprosima::fastdds::dds::DomainParticipant* Interface::GetParticipant() const {
  return participant_;
}

// *=== GetDomainId ===*
int Interface::GetDomainId() const {
  return domain_id_;
}

}  // namespace sturdds