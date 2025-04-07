import sys
import os
from signal import signal, pause, SIGINT
from time import time_ns, sleep
from random import uniform
from threading import Thread

dir = os.path.realpath(os.path.dirname(__file__))
print(dir)
sys.path.append("/usr/local/lib/python3/dist-packages/")
# sys.path.append(dir + "/../build/rtps/sturdds/")
sys.path.append(dir + "/../../../build/src/sturdds/rtps/sturdds/")

import fastdds
import NavMessage


def signal_handler(sig, frame):
    print("Interrupted!")


class Listener(fastdds.DataWriterListener):
    def __init__(self, writer):
        self.writer = writer
        super().__init__()

    def on_publication_matched(self, writer, info):
        if info.current_count_change == 1:
            print(f"Publisher matched a subscriber (Reader: {writer.guid()})")
            self.writer.num_subscribers += 1
        elif info.current_count_change == -1:
            print(f"Publisher unmatched a subscriber (Reader: {writer.guid()})")
            self.writer.num_subscribers -= 1


class Publisher:
    def __init__(self, topic_name):
        factory = fastdds.DomainParticipantFactory.get_instance()
        self.participant_qos = fastdds.DomainParticipantQos()
        factory.get_default_participant_qos(self.participant_qos)
        self.participant = factory.create_participant(0, self.participant_qos)

        self.topic_data_type = NavMessage.NavMessagePubSubType()
        self.topic_data_type.set_name("NavMessage")
        self.type_support = fastdds.TypeSupport(self.topic_data_type)
        self.participant.register_type(self.type_support)

        self.topic_qos = fastdds.TopicQos()
        self.participant.get_default_topic_qos(self.topic_qos)
        self.topic = self.participant.create_topic(
            topic_name, self.topic_data_type.get_name(), self.topic_qos
        )

        self.publisher_qos = fastdds.PublisherQos()
        self.participant.get_default_publisher_qos(self.publisher_qos)
        self.publisher = self.participant.create_publisher(self.publisher_qos)

        self.listener = Listener(self)
        self.writer_qos = fastdds.DataWriterQos()
        self.publisher.get_default_datawriter_qos(self.writer_qos)
        self.writer = self.publisher.create_datawriter(self.topic, self.writer_qos, self.listener)

        self.num_subscribers = 0
        self.running = True

    def __del__(self):
        factory = fastdds.DomainParticipantFactory.get_instance()
        self.participant.delete_contained_entities()
        factory.delete_participant(self.participant)

    def write(self):
        data = NavMessage.NavMessage()
        index = 0
        while self.running:
            sleep(1)
            nanoseconds = time_ns()
            seconds = nanoseconds // 1_000_000_000
            nanoseconds = nanoseconds % 1_000_000_000

            data.stamp().index(index)
            data.stamp().valid(True)
            data.stamp().sec(seconds)
            data.stamp().nanosec(nanoseconds)
            data.Week(1)
            data.ToW(seconds)
            data.Lat(90.0 * uniform(-1.0, 1.0))
            data.Lon(180.0 * uniform(-1.0, 1.0))
            data.H(1.0 * uniform(-1.0, 1.0))
            data.Vn(5.0 * uniform(-1.0, 1.0))
            data.Ve(5.0 * uniform(-1.0, 1.0))
            data.Vd(5.0 * uniform(-1.0, 1.0))
            data.Roll(180.0 * uniform(-1.0, 1.0))
            data.Pitch(90.0 * uniform(-1.0, 1.0))
            data.Yaw(180.0 * uniform(-1.0, 1.0))
            data.Bias(10.0 * uniform(-1.0, 1.0))
            data.Drift(5.0 * uniform(-1.0, 1.0))
            data.P0(1.0 * uniform(-1.0, 1.0))
            data.P1(1.0 * uniform(-1.0, 1.0))
            data.P2(1.0 * uniform(-1.0, 1.0))
            data.P3(1.0 * uniform(-1.0, 1.0))
            data.P4(1.0 * uniform(-1.0, 1.0))
            data.P5(1.0 * uniform(-1.0, 1.0))
            data.P6(1.0 * uniform(-1.0, 1.0))
            data.P7(1.0 * uniform(-1.0, 1.0))
            data.P8(1.0 * uniform(-1.0, 1.0))
            data.P9(1.0 * uniform(-1.0, 1.0))
            data.P10(1.0 * uniform(-1.0, 1.0))

            self.writer.write(data)
            print("Sending Message: ")
            print("---------------- ")
            print(f"Index = {data.stamp().index()}")
            print(f"Valid = {data.stamp().valid()}")
            print(f"Sec   = {data.stamp().sec()}")
            print(f"NSec  = {data.stamp().nanosec()}")
            print(f"Lat   = {data.Lat()}")
            print(f"Lon   = {data.Lon()}")
            print(f"H     = {data.H()}")
            print(f"Vn    = {data.Vn()}")
            print(f"Ve    = {data.Ve()}")
            print(f"Vd    = {data.Vd()}")
            print(f"Roll  = {data.Roll()}")
            print(f"Pitch = {data.Pitch()}")
            print(f"Yaw   = {data.Yaw()}")
            print(f"Bias  = {data.Bias()}")
            print(f"Drift = {data.Drift()}")
            print(f"P0    = {data.P0()}")
            print(f"P1    = {data.P1()}")
            print(f"P2    = {data.P2()}")
            print(f"P3    = {data.P3()}")
            print(f"P4    = {data.P4()}")
            print(f"P5    = {data.P5()}")
            print(f"P6    = {data.P6()}")
            print(f"P7    = {data.P7()}")
            print(f"P8    = {data.P8()}")
            print(f"P9    = {data.P9()}")
            print(f"P10   = {data.P10()}")

            index += 1

    def run(self):
        th = Thread(target=self.write)
        th.start()

        signal(SIGINT, signal_handler)
        print("Press Ctrl+C to stop")
        pause()

        self.running = False
        th.join()


if __name__ == "__main__":
    node = Publisher("navigator")
    node.run()
