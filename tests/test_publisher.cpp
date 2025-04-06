#include <chrono>
#include <memory>
#include <random>
#include <thread>

#include "sturdds/NavMessage.hpp"
#include "sturdds/NavMessagePubSubTypes.hpp"
#include "sturdds/interface.hpp"
#include "sturdds/publisher.hpp"

int main() {
  // random number generator
  std::mt19937 engine(std::random_device{}());
  std::uniform_real_distribution<double> distribution(-1.0, 1.0);

  // create publisher
  NavMessage msg;
  sturdds::Interface node;
  eprosima::fastdds::dds::TopicDataType *type = new NavMessagePubSubType();
  std::unique_ptr<sturdds::Publisher<NavMessage>> pub =
      node.CreatePublisher<NavMessage>("navigator", type);

  int samples_sent = 0;
  int index = 0;
  while (samples_sent < 10) {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);
    msg.stamp().index(index);
    msg.stamp().valid(true);
    msg.stamp().sec(seconds.count());
    msg.stamp().nanosec(nanoseconds.count());
    msg.Week(1);
    msg.ToW(seconds.count());
    msg.Lat(90.0 * distribution(engine));
    msg.Lon(180.0 * distribution(engine));
    msg.H(1.0 * distribution(engine));
    msg.Vn(5.0 * distribution(engine));
    msg.Ve(5.0 * distribution(engine));
    msg.Vd(5.0 * distribution(engine));
    msg.Roll(180.0 * distribution(engine));
    msg.Pitch(90.0 * distribution(engine));
    msg.Yaw(180.0 * distribution(engine));
    msg.P0(1.0 * distribution(engine));
    msg.P1(1.0 * distribution(engine));
    msg.P2(1.0 * distribution(engine));
    msg.P3(1.0 * distribution(engine));
    msg.P4(1.0 * distribution(engine));
    msg.P5(1.0 * distribution(engine));
    msg.P6(1.0 * distribution(engine));
    msg.P7(1.0 * distribution(engine));
    msg.P8(1.0 * distribution(engine));
    msg.P9(1.0 * distribution(engine));
    msg.P10(1.0 * distribution(engine));

    std::cout << "Sending NavMessage: \n-------------------\n";
    std::cout << " Index = " << msg.stamp().index() << "\n";
    std::cout << " Valid = " << msg.stamp().valid() << "\n";
    std::cout << " Sec   = " << msg.stamp().sec() << "\n";
    std::cout << " NSec  = " << msg.stamp().nanosec() << "\n";
    std::cout << " Lat   = " << msg.Lat() << "\n";
    std::cout << " Lon   = " << msg.Lon() << "\n";
    std::cout << " H     = " << msg.H() << "\n";
    std::cout << " Vn    = " << msg.Vn() << "\n";
    std::cout << " Ve    = " << msg.Ve() << "\n";
    std::cout << " Vd    = " << msg.Vd() << "\n";
    std::cout << " Roll  = " << msg.Roll() << "\n";
    std::cout << " Pitch = " << msg.Pitch() << "\n";
    std::cout << " Yaw   = " << msg.Yaw() << "\n";
    std::cout << " Bias  = " << msg.Bias() << "\n";
    std::cout << " Drift = " << msg.Drift() << "\n";
    std::cout << " P0    = " << msg.P0() << "\n";
    std::cout << " P1    = " << msg.P1() << "\n";
    std::cout << " P2    = " << msg.P2() << "\n";
    std::cout << " P3    = " << msg.P3() << "\n";
    std::cout << " P4    = " << msg.P4() << "\n";
    std::cout << " P5    = " << msg.P5() << "\n";
    std::cout << " P6    = " << msg.P6() << "\n";
    std::cout << " P7    = " << msg.P7() << "\n";
    std::cout << " P8    = " << msg.P8() << "\n";
    std::cout << " P9    = " << msg.P9() << "\n";
    std::cout << " P10   = " << msg.P10() << "\n";

    if (pub->Publish(msg)) {
      samples_sent++;
    }
    index++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return 0;
}