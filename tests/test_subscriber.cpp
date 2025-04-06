#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "sturdds/NavMessage.hpp"
#include "sturdds/NavMessagePubSubTypes.hpp"
#include "sturdds/interface.hpp"
#include "sturdds/subscriber.hpp"

void callback(const NavMessage& data) {
  std::cout << "Received NavMessage: \n--------------------\n";
  std::cout << " Index = " << data.stamp().index() << "\n";
  std::cout << " Valid = " << data.stamp().valid() << "\n";
  std::cout << " Sec   = " << data.stamp().sec() << "\n";
  std::cout << " NSec  = " << data.stamp().nanosec() << "\n";
  std::cout << " Lat   = " << data.Lat() << "\n";
  std::cout << " Lon   = " << data.Lon() << "\n";
  std::cout << " H     = " << data.H() << "\n";
  std::cout << " Vn    = " << data.Vn() << "\n";
  std::cout << " Ve    = " << data.Ve() << "\n";
  std::cout << " Vd    = " << data.Vd() << "\n";
  std::cout << " Roll  = " << data.Roll() << "\n";
  std::cout << " Pitch = " << data.Pitch() << "\n";
  std::cout << " Yaw   = " << data.Yaw() << "\n";
  std::cout << " Bias  = " << data.Bias() << "\n";
  std::cout << " Drift = " << data.Drift() << "\n";
  std::cout << " P0    = " << data.P0() << "\n";
  std::cout << " P1    = " << data.P1() << "\n";
  std::cout << " P2    = " << data.P2() << "\n";
  std::cout << " P3    = " << data.P3() << "\n";
  std::cout << " P4    = " << data.P4() << "\n";
  std::cout << " P5    = " << data.P5() << "\n";
  std::cout << " P6    = " << data.P6() << "\n";
  std::cout << " P7    = " << data.P7() << "\n";
  std::cout << " P8    = " << data.P8() << "\n";
  std::cout << " P9    = " << data.P9() << "\n";
  std::cout << " P10   = " << data.P10() << "\n";
}

int main() {
  // create subscriber
  sturdds::Interface node;
  eprosima::fastdds::dds::TopicDataType* type = new NavMessagePubSubType();
  std::unique_ptr<sturdds::Subscriber<NavMessage>> sub =
      node.CreateSubscriber<NavMessage>("navigator", type, &callback);

  // allow messages to be streamed for 15 s
  std::this_thread::sleep_for(std::chrono::milliseconds(15000));

  return 0;
}